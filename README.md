# ReSpring

> A basic animation spring

## Installation

```console
npm install --save re-spring
```

## Example usage

```reason
let animation =
  Animation.spring
  from::10.0
  toValue::0.0
  onChange::(fun (value, _) => {
    CssStyleDeclarationRe.setProperty "top" (string_of_int (float_of_int value) ^ "px") "" style
  })
  ();

animation.start ();
```
