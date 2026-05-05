# Contributing Guidelines

## Commits

- Prefer small commits over large commits
- Break your work into trackable steps
- Try to explain what you do in every commit

## Naming style

We provide a `.clang-tidy` to help you write the code in the style of `ALE`: Stick to it. For the style that cannot be enforced through a `.clang-tidy` file: 

- All variables of the following types should be suffixed with `_w`:
  - `std::optional`, `std::variant`, `std::expected`, `std::any`,
  - `intlib::WrappedAny` 

- All variables of the following types should be suffixed with `_eval` or be named `eval` if that is an appropriate name:
  - `intlib::ast::Evaluation`

- All variables of the following types should be suffixed with `_res` or be named `res` if that is an appropriate name:
  - `intlib::ast::EvaluationResult`

- All variables of the following types should be suffixed with `_err` or be named `err` if that is an appropriate name:
  - `intlib::ast::EvaluationError`

## Coding style

We provide a `.clang-format` file so that your `clang-format` tool can format the code automatically. Stick to it. 
