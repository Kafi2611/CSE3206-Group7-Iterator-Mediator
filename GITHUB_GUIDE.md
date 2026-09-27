# GitHub Guide for Group 7 (Kafi, Sadaf, Sakila)

This guide walks all three of us through putting the project on GitHub. **Each member will make their own commits**, so the repository shows real contributions from everyone. Follow your own part step by step. Every command is written for **Git Bash on Windows**.

| Member | GitHub username | Role on GitHub | Pushes |
|---|---|---|---|
| Abdullah Al Kafi | `Kafi2611` | Repository owner, merges pull requests | project setup + Iterator module + Iterator UML |
| Sadaf Rahman | `sadaf532` | Collaborator | Mediator module + Mediator UML (branch `feature/mediator`) |
| Sakila Akter | `SakilaAkter` | Collaborator | Unit tests + PDF documents (branch `feature/tests-docs`) |

**Repository:** `https://github.com/Kafi2611/CSE3206-Group7-Iterator-Mediator`

**Order of work:** Kafi (Part 1) → Sadaf and Sakila (Parts 2 and 3, in any order, even at the same time) → Kafi merges (Part 4) → everyone syncs (Part 5).

---

## Part 0 – One-time setup (everyone)

1. **Install Git for Windows** from <https://git-scm.com/download/win>. Keep the default options. This also installs **Git Bash** and the **Git Credential Manager** (which lets you log in through the browser).
2. **Tell Git who you are.** Open *Git Bash* and run the two commands with **your own** name and **the same email you used for GitHub**. If the email is different, your commits will not count on your GitHub profile.

   ```bash
   git config --global user.name  "Sadaf Rahman"
   git config --global user.email "your-github-email@example.com"
   git config --global init.defaultBranch main
   ```

3. **Check that it worked:**

   ```bash
   git config --global --list
   ```

4. **Get the project files.** Kafi shares the prepared project folder `CSE3206-Group7-Iterator-Mediator` as a ZIP file, `02_Source_Code_Group7.zip` (on Google Drive or Messenger). Extract it anywhere, for example to your Desktop. We call this extracted folder **"the project ZIP folder"** below.
5. *(Optional, to run the code)* Install a C++ compiler: **MSYS2** (`pacman -S mingw-w64-ucrt-x86_64-gcc`) or the **MinGW** that ships with Code::Blocks. Make sure `g++ --version` works in a new terminal.

> **Golden rule:** do **not** run `git init` inside the project ZIP folder. We **clone** the repository into a *separate* folder and copy **only our own files** into it.

---

## Part 1 – Kafi: create the repository and push the base project

### 1.1 Create the empty repository on GitHub

1. Go to <https://github.com/new>.
2. **Repository name:** `CSE3206-Group7-Iterator-Mediator`
3. **Description:** `CSE 3206 Lab 3 (Group 7): Iterator & Mediator design patterns in C++17 – CampusConnect`
4. Choose **Public**.
5. Do **NOT** tick *Add a README*, *.gitignore* or *license*. We already have these files.
6. Click **Create repository**.

### 1.2 Invite Sadaf and Sakila

Open the repository → **Settings** → **Collaborators** → **Add people** → type `sadaf532` and invite them → do the same for `SakilaAkter`.
They will get an email and a notification, and must click **Accept invitation**.

### 1.3 Clone the empty repository

```bash
mkdir -p /e/RUET/GitHub          # E:\RUET\GitHub  (any folder is fine)
cd /e/RUET/GitHub
git clone https://github.com/Kafi2611/CSE3206-Group7-Iterator-Mediator.git
cd CSE3206-Group7-Iterator-Mediator
```

(Git will warn *"You appear to have cloned an empty repository"*. That is expected.)

### 1.4 Copy ONLY your files into the clone

From the project ZIP folder, copy these into `E:\RUET\GitHub\CSE3206-Group7-Iterator-Mediator` using File Explorer:

| Copy this | Notes |
|---|---|
| `README.md`, `GITHUB_GUIDE.md`, `CMakeLists.txt`, `build.bat`, `build.sh` | root files |
| `.gitignore`, `.gitattributes` | hidden files: in File Explorer turn on *View → Show → Hidden items* |
| `.github\` (whole folder) | the CI workflow |
| `iterator\` (whole folder) | the Iterator module |
| `docs\uml\iterator_*` (6 files) | put them inside `docs\uml\` |
| `docs\uml\source\umlgen.py`, `iterator_class.py`, `iterator_class.dot`, `seqsvg.py`, `render.py` | put them inside `docs\uml\source\` |

### 1.5 Commit in three small, meaningful commits

```bash
git status                     # see what is new

git add README.md GITHUB_GUIDE.md CMakeLists.txt build.bat build.sh .gitignore .gitattributes .github
git commit -m "Set up project: README, team guide, CMake, build scripts and CI"

git add iterator
git commit -m "Add Iterator pattern: CampusConnect feed (friends, suggestions, news feed)"

git add docs/uml
git commit -m "Add Iterator UML class, sequence and motivation diagrams"

git log --oneline              # you should see 3 commits
```

### 1.6 Push

```bash
git branch -M main
git push -u origin main
```

The first push opens a browser window. **Sign in to GitHub** and allow access.
Refresh the repository page: the README appears, and the **Actions** tab shows the build running (green ✓ when it passes).

---

## Part 2 – Sadaf: add the Mediator module via a pull request

### 2.1 Accept the invitation and clone

Accept the invitation from your email or from <https://github.com/notifications>, then:

```bash
mkdir -p /e/RUET/GitHub  &&  cd /e/RUET/GitHub          # or /d/..., any drive
git clone https://github.com/Kafi2611/CSE3206-Group7-Iterator-Mediator.git
cd CSE3206-Group7-Iterator-Mediator
git switch -c feature/mediator                           # your own branch
```

### 2.2 Copy ONLY your files

| Copy this from the project ZIP folder | Into the clone |
|---|---|
| `mediator\` (whole folder) | `mediator\` |
| `docs\uml\mediator_*` (6 files) | `docs\uml\` |
| `docs\uml\source\mediator_class.py`, `mediator_class.dot`, `diagrams.py` | `docs\uml\source\` |

### 2.3 Commit and push your branch

```bash
git add mediator
git commit -m "Add Mediator pattern: CampusConnect Messenger (server, members, support bot)"

git add docs/uml
git commit -m "Add Mediator UML class, sequence and motivation diagrams"

git push -u origin feature/mediator
```

### 2.4 Open a pull request

On GitHub a yellow banner appears: **"feature/mediator had recent pushes – Compare & pull request"**. Click it.
- Title: `Add Mediator pattern (CampusConnect Messenger)`
- Description: one or two lines about what you added.
- Reviewer (right side): `Kafi2611`
- Click **Create pull request**. Wait for the green ✓ from the CI check.

---

## Part 3 – Sakila: add the tests and documents via a pull request

### 3.1 Accept the invitation and clone

```bash
mkdir -p /e/RUET/GitHub  &&  cd /e/RUET/GitHub
git clone https://github.com/Kafi2611/CSE3206-Group7-Iterator-Mediator.git
cd CSE3206-Group7-Iterator-Mediator
git switch -c feature/tests-docs
```

### 3.2 Copy ONLY your files

| Copy this from the project ZIP folder | Into the clone |
|---|---|
| `tests\` (whole folder) | `tests\` |
| `docs\Group7_Lab3_Documentation.pdf` | `docs\` |
| `docs\Group7_Code_Review_Report.pdf` | `docs\` |

### 3.3 Commit, push and open a pull request

```bash
git add tests
git commit -m "Add 33 unit tests for Iterator and Mediator with a tiny test framework"

git add docs/Group7_Lab3_Documentation.pdf docs/Group7_Code_Review_Report.pdf
git commit -m "Add lab documentation and code review report (PDF)"

git push -u origin feature/tests-docs
```

Then open a pull request exactly as in step 2.4. Title: `Add unit tests and documentation`, reviewer `Kafi2611`.

> The tests for a module only build once that module is on `main`. If Sakila's pull request is merged before Sadaf's, the Mediator tests start running automatically as soon as the Mediator pull request is merged.

---

## Part 4 – Kafi: review and merge the pull requests

1. Open the repository → **Pull requests**.
2. Open each pull request → **Files changed** tab → read the code. You can leave a comment on a line (this is real code review!) → **Review changes → Approve**.
3. When the CI check is green, click **Merge pull request → Confirm merge**.
4. Optionally click **Delete branch** after merging.

Because each of us added *different* files, there will be **no merge conflicts**.

---

## Part 5 – Everyone: sync your computer with `main`

```bash
cd /e/RUET/GitHub/CSE3206-Group7-Iterator-Mediator
git switch main
git pull
./build.sh run        # or: build.bat run   (in Command Prompt)
```

You should see `16 passed` and `17 passed`, followed by both demos.

**Final check (Kafi):**

- [ ] The repository page shows the README with both UML diagrams.
- [ ] **Actions** tab: the latest run on `main` is green ✓.
- [ ] **Insights → Contributors** shows commits from all three members.
- [ ] `docs/` contains both PDFs, and the links in the README open them.
- [ ] *(Optional)* **Releases → Create a new release** → tag `v1.0` → title `Lab 3 submission`.

---

## Everyday workflow (if we need to change something later)

```bash
git switch main
git pull                                   # 1. always start from the latest main
git switch -c fix/short-description        # 2. make a branch
# ... edit files ...
git add <files>                            # 3. stage
git commit -m "Explain what and why"       # 4. commit
git push -u origin fix/short-description   # 5. push, then open a pull request
```

**Commit message tips:** start with a verb (*Add*, *Fix*, *Update*, *Refactor*), keep it under about 70 characters, and describe **what** changed.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| `remote: Permission to ... denied` / `403` | Accept the invitation first. If the wrong account is saved: *Windows Credential Manager → Windows Credentials →* remove `git:https://github.com`, then push again. |
| `fatal: not a git repository` | You are in the wrong folder. `cd` into the cloned folder. |
| `Updates were rejected because the remote contains work that you do not have` | Run `git pull --rebase`, then `git push` again. |
| `warning: LF will be replaced by CRLF` | Harmless. `.gitattributes` handles line endings. |
| CI shows a red ✗ | Open **Actions** → click the failed run → read the red step, fix it locally, commit and push again. |
| `g++ is not recognized` | Install MSYS2 or MinGW and add its `bin` folder to `PATH`, then open a new terminal. |
| Merge conflict (`<<<<<<<` markers in a file) | Open the file, keep the correct lines, delete the markers, then `git add <file>` and `git commit`. Ask Kafi if unsure. |

### Prefer buttons over commands? Use GitHub Desktop

<https://desktop.github.com> → *File → Clone repository* → choose the repo → *Current branch → New branch* → copy your files in → write a summary → **Commit** → **Publish branch** → **Create Pull Request**. The result is the same.
