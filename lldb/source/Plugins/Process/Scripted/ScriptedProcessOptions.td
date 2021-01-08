include "../../../Commands/OptionsBase.td"

let Command = "process scripted load" in {
  def scripted_process_shlib : Option<"shlib", "S">, Arg<"ShlibName">,
    Completion<"Module">,
    Desc<"Name of the Python module that holds the scripted porcess.">;
}
