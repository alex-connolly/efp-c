A tool for generating Element-Field Parsers.

```go
fireVM {
    imports! {
        instructions : id!
        fuel : id!
    }
    instruction {
        name : string!
        arguments : int(0)
        push : int(0)
        pop : int(0)
        fuel : (id|int)(0)
        fuel() : id
        execute() : id!
    }
    $ {
        name : string!
    }
    name : string("Default FireVM")
    author : [string]
}
```
