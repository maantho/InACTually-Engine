# ✒️ Contributing

First off, thanks for taking the time to contribute! 🎉 \
Whether you are fixing a bug, suggesting an improvement, or just asking a question, you are helping make this better. \
Let’s make something great together. 🚀

We will extend this contributing guideline if needed.


<br>

## ⚠️ Public Attribution

Please note: Contributions are public and attributed to your GitHub account. If you'd prefer to stay anonymous, feel free to reach out or submit under a pseudonymous fork. Otherwise your (GitHub) name will be listed as contributor and/or in the credits ;) \
As a community we use a centralized copyright `Copyright (c) <year> InACTually Community` in the files. We will have a list of authors/contributors.


<br>

## 🪲 Got an Idea or Found a Bug?

- Use [Issues](https://github.com/InACTually/InACTually-Engine/issues) to share bugs, ideas, or feedback.
- Be clear and include relevant context, logs, or screenshots when possible.


<br>

## 📬 How to Contribute

At first, best is always to contact us, but in the end it boils down to:

1. **Fork** this repo and create your branch off of `main` or better from a special `develop`. Branch name is best like `dev-feature_MyFeature` 
2. **Code cleanly** and write clear commit messages (see below). 
3. **Open a pull request** with a short description of what you did.

As a community we use centralized copyright `Copyright (c) <year> InACTually Community`.
So, if you have significantly contributed to a file, please add your name (as you like) to the "contributors"-list of that file.
If you have created a new file, please add our standard notice at the beginning of the file (as you can find in all others) and start the "contributors"-list with your name. \
Do not forget to change the "modified"-year of that file :)

To contribute, make sure your commits include a Signed-off-by line like `Signed-off-by: Name <email>`. You can add this automatically with:

    `git commit -s`

By doing so, you certify the following:
### Developer Certificate of Origin (DCO)

By making a contribution to this project, I certify that:

1. The contribution was created in whole or in part by me and I have the right to submit it under the open source license indicated in the repository.

2. The contribution is based upon previous work that, to the best of my knowledge, is covered under an appropriate open source license and I have the right under that license to submit that work with modifications, whether created in whole or in part by me.

3. The contribution was provided directly to the project by me without any restriction and without expectation of compensation.

4. I understand and agree that this project and the contribution are public and that a record of the contribution (including all personal information I submit with it, such as my name and email address) is maintained indefinitely and may be redistributed consistent with this project’s license.


<br>

## 🪄 Code Style

- Keep code readable and consistent.
- Comments are welcome, especially for tricky parts.
- Care about naming conventions and terminology, e.g. base classes have `Base` as postfix like `MyClassBase`, member variables start with a `m_` like `m_myMemberVar`.
- Classes (esp. any type of `Node`) should have a corresponding `Ref` declaration like `using std::shared_ptr<MyClass> MyClassRef;` und a corresponding `static create` function that *makes* (`std::make_shared`) the object.


<br>

## 🫂 Be Kind

This project follows a **Code of Conduct**. Be respectful and constructive, everyone is welcome here.

