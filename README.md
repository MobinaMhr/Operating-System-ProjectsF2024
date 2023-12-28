# Operating-System-ProjectsF2024
* CA2. [MapReduce](https://github.com/MobinaMhr/Operating-System-ProjectsF2024/tree/main/CA2-MapReduce)

## Building Utility Consumption Analyzer

### Introduction
This project focuses on the management of building utility processes and their communication methods. The exercise involves performing computational operations on data obtained from building meters. The implementation utilizes the Map-Reduce model, particularly focusing on the mapping and reduction phases. The project addresses the mapping-reduction model, emphasizing its significance in handling large datasets in a distributed environment.

### Map-Reduce Model
In today's world, the exponential growth of data, especially with the advent of the Internet and smart devices, requires efficient processing methods. The Map-Reduce model is a programming framework that enables parallel and distributed processing of extensive datasets. This project delves into the concepts of mapping and reduction to analyze utility consumption in various buildings.

### Project Description
The goal of this project is to calculate statistical indices related to the consumption of electricity, gas, and water in factory buildings. The provided dataset includes consumption data for each resource in different buildings over the past two months, recorded at various hours. The project tasks involve calculating peak consumption hours for each resource and determining the average consumption for each building.

### Implementation Details
Each building has a corresponding folder named after it, containing three CSV files for each resource (electricity, gas, and water). Each file holds resource consumption data, with the resource name matching the file name.

The implementation involves an initial process that receives the "buildings" folder path as a command-line argument. This process identifies and lists all available buildings, prompts the user to select one or more resources, and then creates child processes for each selected building. Each building process, in turn, spawns mapping processes for each resource, enabling parallel execution.

The calculation of bills for each resource involves specific formulas based on peak hours and average consumption.

### How to Run
To execute the program, run the initial process with the "buildings" folder path as a command-line argument. Follow the interactive prompts to select resources and buildings.

### Notes
This program ensures interactivity, allowing users to specify the desired report format.
The user defines the final report, indicating the chosen resource, building, and relevant indices.
