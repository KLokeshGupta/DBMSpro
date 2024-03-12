# NITCbase

NITCbase is an object-oriented implementation of a relational database management system (RDBMS) created as part of a college project.

![NITCbase Logo](https://nitcbase.github.io/assets/images/FrontBannerLogo-0073fc4a8e9ba1da89480779ce0131e0.png)

## Overview
NITCbase offers a user-friendly interface to interact with the RDBMS. You can explore the list of available commands and features in the [User Interface Commands Documentation](User_Interface_Commands_Documentation.md).

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Getting Started](#getting-started)
3. [Installation](#installation)
4. [Running NITCBase](#running-nitcbase)
5. [Usage](#usage)
6. [XFS-Documentation](#xfs-documentation)
7. [Contributing](#contributing)
8. [License](#license)

## Prerequisites
Before running NITCBase, ensure that you have the following prerequisites installed on your system:
- Linux-based operating system (tested on Ubuntu)
- C/C++ compiler (gcc/g++)
- Git
## Installation
To get started with NITCbase, follow these installation steps:

1. Clone the repository:

    ```bash
    git clone https://github.com/KLokeshGupta/DBMSPro.git
    ```

2. Navigate to the project directory:

    ```bash
    cd NITCbase/mynitcbase
    ```

3. Build the project using the `make` command:

    ```bash
    make
    ```

You can also use the following make options:

- `make clean`: Clean up the project.
- `make mode=debug`: Create a debuggable version.

## Usage
Once the installation is complete, you can use NITCbase to interact with the RDBMS. NITCbase provides a user-friendly command-line interface to work with relational databases. You can refer to the [User Interface Commands Documentation](User_Interface_Commands_Documentation.md) to learn about the available commands and their usage.

For detailed usage instructions and information about the XFS interface, please refer to the [Xfs Interface README](XFS_Interface/README.md). You can find instructions on how to build, clean, and debug the XFS interface there.

Be aware that the XFS interface operates directly on the disk without creating a run copy.

## Project Structure
- **Disk/**: Contains binary files disk and disk_run_copy.
- **Files/**: Contains the input and output files. You can also write scripts in the Batch_exec_files folder to execute multiple commands at once.
- **mynitcbase/**: The main project directory.
- **XFS_Interface/**: Contains the source code for the XFS interface.

## Contributing
We welcome contributions from the open-source community. If you'd like to contribute to NITCbase, please follow our [Contribution Guidelines](CONTRIBUTING.md).

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact
For questions, feedback, or support, please feel free to contact us:

- Email: lokeshguptakusumanchi@gmail.com
- Project Website: [NITCbase](https://nitcbase.github.io)
- GitHub Repository: [NITCbase Repository](https://github.com/KLokeshGupta/DBMSPro)

Thank you for your interest in NITCbase!
