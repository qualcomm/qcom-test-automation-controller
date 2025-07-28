## Contributing to Qualcomm Test Automation Controller (QTAC)

Hi there!

We’re thrilled that you’d like to contribute to this project.
Your help is essential for keeping this project great and for making it better.

## Branching Strategy
In general, contributors should develop on branches based off of `develop` and pull requests should be made against `develop`.

## Submitting a pull request
1. Please read our [code of conduct](CODE-OF-CONDUCT.md) and [license](LICENSE).

2. [Fork](https://github.com/qualcomm/qcom-test-automation-controller/fork) and clone the repository.
    ```bash
    git clone https://github.com/<username>/qcom-test-automation-controller.git
    ```
3. Create a new branch based on `develop`:
    ```bash
    git checkout -b <my-branch-name> develop
    ```
4. Create an upstream `remote` to make it easier to keep your branches up-to-date:
    ```bash
    git remote add upstream https://github.com/qualcomm/qcom-test-automation-controller.git
    ```

5. Make your changes, add tests, and make sure tests passes.

6. Commit your changes using the [DCO](http://developercertificate.org/). You can attest to the DCO by commiting with the **-s** or **--signoff** options or manually adding the "Signed-off-by":
    ```bash
    git commit -s -m "Really useful commit message"`
    ```

7. After committing your changes on the topic branch, sync it with the upstream branch:
    ```bash
    git pull --rebase upstream main
    ```

8. Push to your fork.
    ```bash
    git push -u origin <my-branch-name>
    ```

    The `-u` is shorthand for `--set-upstream`. This will set up the tracking reference so subsequent runs of `git push` or `git pull` can omit the remote and branch.

9. [Submit a pull request](https://github.com/qualcomm/qcom-test-automation-controller/pulls) from your branch to `develop`.

10. Pat yourself on the back and wait for your pull request to be reviewed.

Here are a few things you can do that will increase the likelihood of your pull request to be accepted:

- Use GitHub issues to report bugs, propose features, suggest architecture adjustments, or make modifications to the core codebase.
- Provide documentation to support any changes that alter existing behavior.
- Follow [LLVM coding conventions](https://llvm.org/docs/CodingStandards.html) when contributing new code to the repository.
> **_NOTE:_** The current codebase incorporates a mix of coding styles, and we are in the process of transitioning to LLVM coding standards for consistency.
- Write unit tests.
- Keep your change as focused as possible.
  If you want to make multiple independent changes, please consider submitting them as separate pull requests.
- Write a [good commit message](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html).
- It's a good idea to arrange a discussion with other developers to ensure there is consensus on large features, architecture changes, and other core code changes. PR reviews will go much faster when there are no surprises.