# Budgeted Coverage Project
## Project Overview
The budgeted maximum coverage problem is defined as follows. A company has released a smartphone app that participants can install.
This app allows participants to send the company pollution data, the phone's location, and a bid price.
The company must select participants in a way that maximizes the data coverage while minimizing costs.
- A 100x100 grid is used to store the locations of phone sensors. A predetermined number of phones are randomized across the grid.
- The amount of area covered by each phone is the same. The coverage of each phone is given as a circle with a predetermined radius, with the phone at the center. 
- Covered phones are those within the coverage area of a purchased phone sensor, including the purchased phone itself.
- Phones are grouped into neighberhoods, some wealthier than others. Phones will have different bid prices depending on the neigberhood they are in.
- The number of unique phones covered should be maximized, so phones that overlap are undesirable.
- Given a budget, a set of phone sensors should be purchased that maximizes the number of unique phones covered.

Below is an example of a purchased phone covering three phones and an example of four purchased phones covering five phones.

![picture alt](https://github.com/SuperLan11/BudgetedCoverage/blob/master/Coverage1.jpeg)

![picture alt](https://github.com/SuperLan11/BudgetedCoverage/blob/master/Coverage2.jpeg)

## Team Contributions
- Craig White:
  - Sensor struct
  - random distribution functions
  - greedyAlgorithm()
  - countBits()
  - namespace std manipulation
  - recursiveSolve()
  - dynamicAlgorithm()
  - random sensor placement functions
  - generateSensorsClustered()
  - contains()
  - calculateTotalCoverage()
  - calculateAreaCoverage
  - TrialResult class
  - runTrial()
  - experiment()
  - Python plotter file
- Landon Johnson:
  - chooseSensorsRandomly()
  - returnCoveredSensors()
  - sortSensors()
  - budgetAlgorithm()
  - main()
- A.L. Seppala:
  - sortSensorsByWeight()
  - weightedAlgorithm()
- David Wonggant: Presentation Lead

# About the plotter:
The plotter is a python program, which plots the data using the matplotlib library.

You will need to install python if you haven't already.  When you're installing it, be sure to check the "Add to PATH" option or "Add to Environment Variables".  It's something like that.

To install matplotlib, run `pip install matplotlib` in your terminal.

Then you can run the python program with `py plotter.py`.

The program will attempt to read `output.txt`, which is generated by the c++ program, so run the c++ program first.

The output file has 3 sections
- `Sensors`: the sensor locations
- `Chosen`: the chosen sensors
- `R`: The radius of each sensor

I will post an example of this file under `output_example.txt`
