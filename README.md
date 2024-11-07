# GoblinPrompt 🧙‍♂️

Welcome to **GoblinPrompt**—a mischievous take on your classic Unix shell, designed by goblins, for goblins! Inspired by the ancient rules of goblin society, this little shell does the basics, with a twist of goblin wisdom. From command parsing to piping and I/O redirection, GoblinPrompt has it all, but don't expect it to behave quite like human shells... goblins have rules, and they mean business!

## Features 🛠️
- **Command Execution**: Run commands goblin-style, with built-in `cd`, `bye`, and `path` commands for a classic, yet quirky experience.
- **Pipeline Support**: String together multiple commands with pipes (|), and let the magic of goblin engineering do the rest.
- **I/O Redirection**: Perform input (`<`) and output (`>`) redirection like a seasoned goblin mage.
- **Goblin Rules Enforcement**: Includes the legendary goblin rules of conduct—because goblins don't mix with humans, elves, or adventurers.

## Goblin Rules 📜
In GoblinPrompt, certain commands are restricted by the Goblin Rules:
- Goblins don’t trust humans. (Or elves. Or adventurers.)
- Goblins stick to their own... mostly. Any attempt to breach these rules might result in unpredictable, amusing outcomes!

## Installation 📥

Clone the repo and step into goblin territory:
```bash
git clone https://github.com/yourusername/GoblinPrompt.git
cd GoblinPrompt
```
Compile the code:
```bash
gcc -o GoblinPrompt GoblinPrompt.c
./GoblinPrompt
```
## Usage 🚀

### Basic Commands
Run commands just like in any Unix shell, but with a goblin attitude.
```bash
GoblinPrompt> echo Hello, Goblin World!
Hello, Goblin World!
```
*Pipeline Magic
GoblinPrompt can handle multiple pipes. Try commands like:
```bash
GoblinPrompt> ls | grep "project" | wc -l
```
*I/O Redirection
Redirect input and output with goblin ease:
Output Redirection: Save command output to a file.
```bash
GoblinPrompt> ls > output.txt
```
After running this, output.txt will contain the list of files from the ls command.
Input Redirection: Use the contents of a file as input for a command.
```bash
GoblinPrompt> cat < output.txt
```
This will read from output.txt and display its contents in GoblinPrompt.
Combine with pipes for even more goblin magic!
```bash
GoblinPrompt> ls | grep "goblin" > goblin_files.txt
GoblinPrompt> wc -l < goblin_files.txt
```
Basically, any Unix-like command can be used in GoblinPrompt, with a dash of goblin flair. Explore and have fun, but remember the goblin rules!
