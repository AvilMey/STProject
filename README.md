# STProject
This project was implemented for the course "Selected Topics in Music and Acoustic Engineering" in the Politecnico di Milano. 

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation and Setup](#installation-and-setup)
- [Usage](#usage)

## Project Overview

STProject consists in a flamenco rhythm generator based on Variable Markov Models. The main goal is the creation of a plugin that accurately generates flamenco rhythms, imitating the style and conditioned by the input secuencies that will enter the plugin. 
Regarding our system architecture, it will receive a MIDI file as input and it will generate a MIDI file with the secuence of notes as output.

## Features

This project is implemented entirely in the Visual Studio 2022 environment using C++ as the programming language. These elements will allow us to implement the Markov Models and interacting with the MIDI data, as well as debugging our project when it is needed in order to test it.
On the other hand, the framework JUCE will give us the tools to finish and properly implement a functional plugin that achieves the established goals. Particularly, the Audio Plugin Host, which is one of the many applications JUCE offers, will be highly useful for testing in a more realistic environment. One the pluin is build, it should work in the DAW desired by the user

## Getting Started

This section will cover the set up of the necessary software to run the code successfully.

### Prerequisites

Outline any software, libraries, or tools that need to be installed before running your code. Include versions if applicable.
It is required to have installed the JUCE libreries corresponding to the implementation of the plugin (https://juce.com/download/).
Also, in order to launch and debug the code, Microsoft Visual Studio 2022 with the libraries corresponding to C++.


### Installation and Setup

Provide step-by-step instructions for setting up the project. This could involve cloning the repository, installing dependencies, and any other necessary setup procedures.
Once the prerequisites are fulfilled, the project can be cloned as follows:

```bash
# Clone the repository
git clone https://github.com/AvilMey/STProject.git

# Navigate to the project directory
cd STProject
```
First of all, you have to make sure the JUCE path in the CMakeList is the correct one, otherwise it way not work. It should be the path directing to the folder where the modules of JUCE are found.
Once the project prepared, in order to launch it, Microsoft Visual Studio will be used but it should be runned as administrator.

When the project is launched, the first thing to do is build it so the plugin can compile. Open the terminal into the project folder and execute the following command so all of the executables are created:

```bash
# Clone the repository
cmake -B build
```

Finally, the plugin needs to be compiled. For that, add the "markov-plugin_VST3" in the debug settings and compile it. This way, the plugin "markov-plugin" will appear in the default VST3 folder of your computer and you can start using it.

### Usage

In the DAW of your choice, add it to a MIDI track. The input will enter the plugin and the MIDI output will be generated based on the input. An interface will be showed containing a MIDI keyboard and two buttons. 
The keyboard allows you to enter new additional MIDI notes and will interfere in the output generating new notes in the secuencie. In the code it is also implemented the functionality of modelling chords (i.e. more than one note at once can be generated) and also modelling the duration of the notes. 
The "Reset" button will reset the model so it will start learning from zero.
The "Machine is learning" button will indicate whenever the model is learning. If pressed, it will stop learning and it will notice the user by changing the color as well as the text on it for "Not Learning".

