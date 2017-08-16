type springCommands = {
  start: unit => unit,
  stop: unit => unit
};

type t = (float, float);

let spring:
  stiffness::float? =>
  damping::float? =>
  from::(float, float) =>
  toValue::float =>
  onChange::(t => 'a) =>
  onRest::(unit => unit)? =>
  unit =>
  springCommands;
