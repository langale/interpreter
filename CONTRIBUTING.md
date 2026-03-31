# Contributing Guidelines

## Commits

- Prefer small commits over large commits
- Break your work into trackable steps
- Try to explain what you do in every commit

## Naming style

We provide a `.clang-tidy` to help you write the code in the style of `ALE`. Stick to it. For the style that cannot be enforced through a `.clang-tidy` file: 

- Variables of type `std::optional`, `std::variant`, `std::expected` or `std::any` should be suffixed with `_w`.

## Coding style

We provide a `.clang-format` file so that your `clang-format` tool can format the code automatically. Stick to it. 
