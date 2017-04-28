# Element-Field Parser

A tool for generating and running element-field parsers.

## What is an element-field parser?

An element-field structure is composed of elements:

```
element {
    key = value // and fields!
}
```
They allow for the easy representation of hierachical data structures through nesting.

## How does it work?

The ```efp``` uses two trees to parse a file. The first tree is the "prototype" tree. This tree defines the specification for all files parsed during this instance. See the Prototype Tree section for more.


# Prototype

### Data Types

Currently, the ```efp``` supports 5 data types:

```c
int         // integers
string      // character strings
float       // floating-point numbers
id          // identifiers
boolean     // booleans

```

### Arrays

```efp``` supports arbitrary-length array structures, which can be represented as follows:

```c
[int]       // integer array (no restrictions)
[2:int]     // integer array (min. length = 2)
[int:2]     // integer array (max. length = 2)
[2:int:2]   // integer array (min./max. length = 2)
```

### Default Values

Fields can have default values which are automatically inserted if they are left blank:

```c
parent {
    data : string("default_key")
    number : int(0)
    identifier : id(hello)
    is_default : boolean(true)
}
```

### Compulsory Fields

Fields are optional by default. In order to force the inclusion of a particular field, simply add the require operator ```!``` to the end of the declaration:

```c
parent {
    name : string!
}
```

### Variable Names

Variable elements can be defined as follows:

```
<a-zA-Z:5> {

}
```

Elements and fields are once-only by default. To allow an element to be repeated 5 times, use the following syntax:

```
<a-zA-Z:5> {

}
```

To allow an arbitrary number of duplicate elements:

```c
<"a-zA-Z":?>{

}
```

# What gets produced?

The following C structure is generated:

```c
struct element {

}
```

The following interfaces are provided:

```c
struct element* element_element(char* key, int index);
struct field* element_field(char* key, int index);
union field_value* field_value(struct field* field, int index);
```

For a working implementation of a VM-generator based on the ```efp```, check out https://github.com/end-r/fireVM.

# Errors

## Prototype Errors



## Parse Errors

```c
// thrown when there is
"Invalid element key"
//
```


# Examples

```go
fireVM {
    variables {
        # = int|bool|string|float|[array]|
    }
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
