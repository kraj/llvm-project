//===- Origins.cpp - Origin Implementation -----------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/LifetimeSafety/Origins.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/TypeBase.h"
#include "clang/Analysis/Analyses/LifetimeSafety/LifetimeAnnotations.h"

namespace clang::lifetimes::internal {

bool isPointerLikeType(QualType QT) {
  return QT->isPointerOrReferenceType() || isGslPointerType(QT);
}

static bool hasOrigins(QualType QT) { return isPointerLikeType(QT); }

/// Determines if an expression has origins that need to be tracked.
///
/// An expression has origins if:
/// - It's a glvalue (has addressable storage), OR
/// - Its type is pointer-like (pointer, reference, or gsl::Pointer)
///
/// Examples:
/// - `int x; x` : has origin (glvalue)
/// - `int* p; p` : has 2 origins (1 for glvalue and 1 for pointer type)
/// - `std::string_view{}` : has 1 origin (prvalue of pointer type)
/// - `42` : no origin (prvalue of non-pointer type)
/// - `x + y` : (where x, y are int) → no origin (prvalue of non-pointer type)
static bool hasOrigins(const Expr *E) {
  return E->isGLValue() || hasOrigins(E->getType());
}

void OriginManager::dump(OriginID OID, llvm::raw_ostream &OS) const {
  OS << OID << " (";
  Origin O = getOrigin(OID);
  if (const ValueDecl *VD = O.getDecl()) {
    OS << "Decl: " << VD->getNameAsString();
  } else if (const Expr *E = O.getExpr()) {
    OS << "Expr: " << E->getStmtClassName();
    if (auto *DRE = dyn_cast<DeclRefExpr>(E)) {
      if (const ValueDecl *VD = DRE->getDecl())
        OS << "(" << VD->getNameAsString() << ")";
    }
  } else {
    OS << "Unknown";
  }
  OS << ")";
}

template <typename T>
OriginTree *OriginManager::buildTreeForType(QualType QT, const T *Node) {
  assert(isPointerLikeType(QT) &&
         "buildTreeForType called for non-pointer type");
  OriginTree *Root = createNode(Node);
  if (QT->isPointerOrReferenceType()) {
    QualType PointeeTy = QT->getPointeeType();
    // We recurse if the pointee type is pointer-like, to build the next
    // level in the origin tree. E.g., for T*& / View&.
    if (isPointerLikeType(PointeeTy))
      Root->Pointee = buildTreeForType(PointeeTy, Node);
  }
  return Root;
}

OriginTree *OriginManager::getOrCreateTree(const ValueDecl *D) {
  if (!isPointerLikeType(D->getType()))
    return nullptr;
  auto It = DeclToTree.find(D);
  if (It != DeclToTree.end())
    return It->second;
  return DeclToTree[D] = buildTreeForType(D->getType(), D);
}

OriginTree *OriginManager::getOrCreateTree(const Expr *E) {
  if (auto *ParenIgnored = E->IgnoreParens(); ParenIgnored != E)
    return getOrCreateTree(ParenIgnored);

  if (!hasOrigins(E))
    return nullptr;

  auto It = ExprToTree.find(E);
  if (It != ExprToTree.end())
    return It->second;

  QualType Type = E->getType();

  // Special handling for DeclRefExpr to share origins with the underlying decl.
  //
  // For reference-typed declarations (e.g., `int& r`), the DeclRefExpr
  // directly reuses the declaration's tree since references don't add an
  // extra level of indirection at the expression level.
  //
  // For non-reference declarations (e.g., `int* p`), the DeclRefExpr is an
  // lvalue (addressable), so we create an outer origin for the lvalue itself,
  // with the pointee being the declaration's tree. This models taking the
  // address: `&p` borrows the storage of `p`, not what `p` points to.
  //
  // This ensures origin sharing: multiple DeclRefExprs to the same declaration
  // share the same underlying origins.
  if (auto *DRE = dyn_cast<DeclRefExpr>(E)) {
    OriginTree *Root = nullptr;
    if (DRE->getDecl()->getType()->isReferenceType())
      Root = getOrCreateTree(DRE->getDecl());
    else {
      Root = createNode(DRE);
      Root->Pointee = getOrCreateTree(DRE->getDecl());
    }
    return ExprToTree[E] = Root;
  }
  // If E is an lvalue , it refers to storage. We model this storage as the
  // first level of origin tree, as if it were a reference, because l-values are
  // addressable.
  if (E->isGLValue() && !Type->isReferenceType())
    Type = AST.getLValueReferenceType(Type);
  ExprToTree[E] = buildTreeForType(Type, E);
  return ExprToTree[E];
}

const Origin &OriginManager::getOrigin(OriginID ID) const {
  assert(ID.Value < AllOrigins.size());
  return AllOrigins[ID.Value];
}

} // namespace clang::lifetimes::internal
