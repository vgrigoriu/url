## Instructions for Documentation

There are different possible configurations when building the docs:  

Option 1. Start out with single boost library, such as the one in this repository. A new boost-root directory will be generated for you, next to the current repo, (in the location ../boost-root)  and the docs will be output to ../boost-root/libs/_name-of-this-repo_/doc/html  

or  

Option 2. You have already set up boost-root. The repo has already been placed in boost-root/libs/_name-of-this-repo_, and that's where you will run the build. In that case, the docs will be output in the current directory, such as _name-of-this-repo_/doc/html.  The existing boost-root will be used.  

Either of the above choices are possible. The build scripts detect if they are being run from a boost-root or not.

In order to build the documentation, refer to the appropriate sections below:

## Linux

Run the linuxdocs.sh script which can be found in the doc/tools directory: 
```
tools/linuxdocs.sh
```

## Mac OSX

Run the osxdocs.sh script which can be found in the doc/tools directory:
```
tools/osxdocs.sh
```

## Windows

Run the powershell script windowsdocs.ps1 which can be found in the doc/tools directory:
```
.\tools\windowsdocs.ps1
```

