# ✒️ Contributing

First off, thanks for taking the time to contribute! 🎉 \
Whether you are fixing a bug, suggesting an improvement, or just asking a question, you are helping make this better. \
Let’s make something great together. 🚀

We will extend this contributing guideline if needed.

<br>

## ⚠️ Public Attribution

Please note: Contributions are public and attributed to your GitHub account. If you'd prefer to stay anonymous, feel free to reach out or submit under a pseudonymous fork. Otherwise your (GitHub) name will be listed as contributor and/or in the credits ;)

<br>

## 🪲 Got an Idea or Found a Bug?

- Use [Issues](https://github.com/yourrepo/issues) to share bugs, ideas, or feedback.
- Be clear and include relevant context, logs, or screenshots when possible.

<br>

## 📬 How to Contribute

At first, best is always to contact us, but in the end it boils down to:

1. **Fork** this repo and create your branch off of `main` or better from a special `develop`. Branch name is best like `dev-feature_MyFeature` 
2. **Code cleanly** and write clear commit messages. 
3. **Open a pull request** with a short description of what you did.

<br>

## 🪄 Code Style

- Keep code readable and consistent.
- Comments are welcome, especially for tricky parts.
- Care about naming conventions and terminology, e.g. base classes have `Base` as postfix like `MyClassBase`, member variables start with a `m_` like `m_myMemberVar`.
- Classes (esp. any type of `Node`) should have a corresponding `Ref` declaration like `using std::shared_ptr<MyClass> MyClassRef;` und a corresponding `static create` function that *makes* (`std::make_shared`) the object.

<br>

## 🫂 Be Kind

This project follows a **Code of Conduct**. Be respectful and constructive, everyone is welcome here.

