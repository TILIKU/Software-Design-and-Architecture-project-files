#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <map>
#include <set>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// STUDENT DOCUMENTATION
// =========================================================================

enum TechnicianPhase { 
    LEVEL_6_SURVIVAL_MODE,
    GO_TO_ENGINEER, 
    RUN_LEVEL_5_TECHNICIAN
};

struct EstadoT {
  ubicacion site;
  bool shoes;

  bool operator==(const EstadoT &state) const{
    return site == state.site and shoes == state.shoes;
  }

  bool operator!=(const EstadoT &state) const{
    return !(*this == state);
  }

  bool operator<(const EstadoT &state) const {
    if (site.f != state.site.f) {
        return site.f < state.site.f;
    }
    if (site.c != state.site.c) {
        return site.c < state.site.c;
    }
    if (site.brujula != state.site.brujula) {
        return site.brujula < state.site.brujula;
    }
    return shoes < state.shoes;
  }
};

struct NodoT{
  EstadoT state;
  list<Action> sequence;

  bool operator==(const NodoT &node) const{
    return state == node.state;
  }

  bool operator<(const NodoT &node) const{
    if (state.site.f < node.state.site.f) return true;
    else if (state.site.f == node.state.site.f and state.site.c < node.state.site.c) return true;
    else if (state.site.f == node.state.site.f and state.site.c == node.state.site.c and state.site.brujula <
      node.state.site.brujula) return true;
    else if (state.site.f == node.state.site.f and state.site.c == node.state.site.c and state.site.brujula ==
      node.state.site.brujula and state.shoes < node.state.shoes) return true;
    else return false;
  }
};

struct NodoTCoste {
  EstadoT state;
  list<Action> sequence;
  int cost = 0;      // g(n): Accumulated energy spent
  int heuristicEstimate = 0; // h(n): Estimated energy to the goal

  bool operator==(const NodoTCoste &node) const {
    return state == node.state;
  }
};

struct ComparaCostes {
    bool operator()(const NodoTCoste &a, const NodoTCoste &b) const {
        int fA = a.cost + a.heuristicEstimate;
        int fB = b.cost + b.heuristicEstimate;
        
        // If costs are equal, break ties by prioritizing the node with the LOWEST heuristic.
        // (the one closer to the goal)
        if (fA == fB) {
            return a.heuristicEstimate > b.heuristicEstimate; 
        }
        return fA > fB; // Min-heap normal
    }
};

struct NodoT_AEstrella {
  EstadoT state;
  list<Action> sequence;
  int g; // Cost accumulated from the start
  int f; // g + h (estimated total cost)

  bool operator>(const NodoT_AEstrella &node) const {
    return f > node.f; // Reversed so priority_queue extracts the smallest 'f'
  }
  bool operator<(const NodoT_AEstrella &node) const {
    return f < node.f; // Reversed so priority_queue extracts the smallest 'f'
  }
};

/*
 * CLASS: ComportamientoTecnico
 * 
 * DESCRIPTION:
 * This class implements the Technician agent's behavior in the Belkan world.
 * The technician works with the engineer to solve the pipe installation problem.
 */



class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORS
  // =========================================================================
  
  /**
   * @brief Constructor for levels 0, 1, and 6 (without a complete map)
   * @param size Map size (initialized later if 0)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    last_action = IDLE;
    hasShoes = false;
    left45TurnCount = 0;

    timeMap = vector<vector<int>>(size, vector<int>(size, 0));
  }

  /**
   * @brief Constructor for levels 2, 3, 4, and 5 (with a fully known map)
   * @param terrainMap Known terrain map
   * @param elevationMap Known elevation map
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> terrainMap, 
                       std::vector<std::vector<unsigned char>> elevationMap): 
                       Comportamiento(terrainMap, elevationMap) {
    // Initialize state variables
    hasPlan = false;
    hasShoes = false;
    last_action = IDLE;
    left45TurnCount = 0;

  }

  ComportamientoTecnico(const ComportamientoTecnico &behavior): Comportamiento(behavior) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Main technician decision loop.
   * Read the sensors and choose the next action.
   * 
   * USAGE EXAMPLE:
   * Action action = think(sensors);
   * return action; // The engine will execute this action
   */
  Action think(Sensores sensors);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // STUDENT IMPLEMENTATION AREA
  // =========================================================================
  
  int exitLoop();

  /**
   * @brief Update the time map by increasing the current cell's value and applying decay to adjacent cells.
   * @param f Row of the current cell.
   * @param c Column of the current cell.
   */
  void updateTimeMap(int f, int c);

  /**
   * @brief Determine whether a cell is viable based on collision and elevation.
   * @param collisionCell Terrain type used for collision checking.
   * @param heightCell Terrain type used for elevation checking.
   * @param heightDifference Elevation difference between the current cell and the cell ahead.
   * @return 'P' if the cell is not viable, or the terrain type otherwise.
   */
  char isViable(char collisionCell, char heightCell, int heightDifference);
  
  /**
   * @brief Determine whether a cell is viable for collision.
   * @param cell terrain type
   * @return false if the cell is inaccessible due to collision, and true otherwise
   */
  bool isCollisionViable(char cell);

  /**
   * @brief Determine whether a cell is viable based on elevation.
   * @param cell terrain type
   * @param difference elevation difference between cells
   * @return 'P' if inaccessible due to elevation, or the cell type otherwise
   */
  char isHeightViable(char cell, int difference);

  /**
   * @brief Determine the best option among the three cells ahead.
   * @param i Terrain at surface position 1 (45 degrees left)
   * @param c Terrain at surface position 2 (directly ahead)
   * @param d Terrain at surface position 3 (45 degrees right)
   * @param sensors Current sensor data.
   * @return 2 for WALK, 1 for TURN_SL, and 3 for TURN_SR. 0 if nothing is interesting.
   */
  int chooseInterestingCellLevel0(char i, char c, char d, Sensores sensors);

  int chooseInterestingCellLevel1(char i, char c, char d, Sensores sensors);

  /**
   * @brief Behavior of the technician for level 0.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel0(Sensores sensors);
    
  /**
   * @brief Behavior of the technician for level 1.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel1(Sensores sensors);
    
  /**
   * @brief Behavior of the technician for level 2.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel2(Sensores sensors);
    
  int heuristic(const EstadoT &current, const EstadoT &goal);

  int baseCost(Action action, unsigned char currentTerrain);

  int heightCostModifier(int currentHeight, int destinationHeight, int baseCostValue,  unsigned char currentTerrain);

  EstadoT applyTechnicianAction(Action action, const EstadoT & state, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap);

  list<Action> aStarSearch(const EstadoT &start, const EstadoT &goal, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap, int obsF = -1, int obsC = -1);

  bool isTechnicianCellAccessible(const EstadoT &state, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap);

  EstadoT nextTechnicianCell(const EstadoT &state);

  /**
   * @brief Behavior of the technician for level 3.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel3(Sensores sensors);
    


  















  /**
   * @brief Behavior of the technician for level 4.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel4(Sensores sensors);
    
  /**
   * @brief Behavior of the technician for level 5.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel5(Sensores sensors);
    
  Action engineerBehaviorLevel2(Sensores sensors);

  /**
   * @brief Behavior of the technician for level 6.
   * @param sensors Current sensor data.
   * @return Action to perform.
   */
  Action technicianBehaviorLevel6(Sensores sensors);

  Action reactiveAStarLevel6(const EstadoT &start, const EstadoT &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap);

  int calculateHeuristic(int currentRow, int currentColumn, int goalRow, int goalColumn);

  Action technicianBehaviorLevelE(Sensores sensors);

  list<Action> breadthFirstSearchV2(const EstadoT &start, const EstadoT &goal,
                          const vector<vector<unsigned char>> &terrain,
                          const vector<vector<unsigned char>> &heightMap);
protected:
  // =========================================================================
  // PROVIDED FUNCTIONS
  // =========================================================================

  /**
   * @brief Update the terrain and elevation maps using sensor data.
   * IMPORTANT: This function is already implemented. It updates the terrain and elevation maps
   * using data from all 16 sensors.
   */
  void updateMap(Sensores sensors);

  /**
   * @brief Determine whether a cell is traversable by the technician.
   * NOTE: The technician may have different traversability rules from the engineer.
   * @param f Cell row.
   * @param c Cell column.
   * @param hasShoes Indicates whether the agent has the shoes.
   * @return true if the cell is traversable.
   */
  bool isTraversableCellLevel0(int f, int c, bool hasShoes);

  /**
   * @brief Check whether the cell ahead is accessible based on elevation difference.
   * TECHNICIAN RULE: Maximum elevation change is always 1, regardless of shoes.
   * @param current Current agent state (row, column, orientation).
   * @return true if the elevation difference with the cell ahead is acceptable.
   */
  bool isHeightAccessible(const ubicacion &current);

  /**
   * @brief Return the position (row, column) of the cell directly ahead of the agent.
   * @param current Current agent state (row, column, orientation).
   * @return State containing the row and column of the cell ahead.
   */
  ubicacion cellAhead(const ubicacion &current) const;

  /**
   * @brief Check whether a cell is traversable by default.
   * @param c Character representing the surface type.
   * @return true if it is a path ('C'), shoes ('D'), or goal ('U').
   */
  bool isPath(unsigned char c) const;

    /**
 * @brief Print an agent's action sequence to the console.
 * @param plan  List of actions in the plan.
 */
  void printPlan(const list<Action> &plan);


/**
 * @brief Print the coordinates and operations in a pipe plan.
 * @param plan  List of steps (row, column, operation).
 */
  void printPlan(const list<Paso> &plan);


  /**
 * @brief Convert an action plan into a list of cells for
 *        display on the graphical map.
 * @param state    Starting state.
 * @param plan  List of actions in the plan.
 */
  void visualizePlan(const ubicacion &state, const list<Action> &plan);

private:
  // =========================================================================
  // STATE VARIABLES (MAY BE EXTENDED BY THE STUDENT)
  // =========================================================================

  Action last_action;
  bool hasShoes;
  int left45TurnCount;

  vector<vector<int>> timeMap;
  int currentInstant = 1;

  list<Action> plan;
  bool hasPlan;

  bool hasArrived = false;
  bool hasReceivedCall = false;
  int timeSinceWalk = -1;
  int patienceCounter = 0;
  int destinationRow = -1;
  int destinationColumn = -1;

  TechnicianPhase currentTechnicianPhase = LEVEL_6_SURVIVAL_MODE;
  EstadoT belkanitaGoal;

  set<pair<int, int>> temporaryObstacles;
  vector<vector<unsigned char>> localMap = mapaResultado;
};

#endif
