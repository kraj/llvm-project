# RUN: %PYTHON %s | FileCheck %s

from mlir.dialects import ptr
from mlir.ir import Context, Location, Module, InsertionPoint


def run(f):
    print("\nTEST:", f.__name__)
    with Context(), Location.unknown():
        module = Module.create()
        with InsertionPoint(module.body):
            f(module)
        print(module)
        assert module.operation.verify()


# CHECK-LABEL: TEST: test_smoke
@run
def test_smoke(_module):
    null = ptr.constant(True)
    # CHECK: ptr.constant #ptr.null : !ptr.ptr<#ptr.generic_space>
