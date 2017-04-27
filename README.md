
The fireVM parser is a reasonably simple Element Field Parser (EFP). Elements are hierarchical, and form a key-based tree structure, with descriptive fields. The type of an element is its "key" (e.g):

```
key {

}
```

Within each element there are fields:

```
key {
    key = value
}
```
