#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <queue>
#include <map>

#include "comportamientos/comportamiento.hpp"

enum EngineerPhase { 
    SEARCH_BELKANITA, 
    EXPLORE_MAP, 
    GO_TO_PIPE, 
    RETURN_TO_BELKANITA, 
    RUN_LEVEL_5_ENGINEER
};

struct EstadoI {
  ubicacion site;
  bool shoes;
  bool operator==(const EstadoI &state) const{
    return site == state.site and shoes == state.shoes;
  }
  bool operator!=(const EstadoI &state) const{
    return !(*this == state);
  }
  bool operator<(const EstadoI &state) const {
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

struct Nodo_Nivel4 {
    EstadoI state;
    int currentHeight;
    int previousPipeHeight;
    bool pipeInstalled;
    bool terrainAltered;

    int gInstallCount;
    int hDistance;
    int totalFCost;

    int ecologicalImpact;

    list<Action> actionSequence;
    list<Paso> stepSequence;

    bool operator>(const Nodo_Nivel4 &other) const {
        if (totalFCost != other.totalFCost) {
            return totalFCost > other.totalFCost;
        }
        
        if (ecologicalImpact != other.ecologicalImpact) {
            return ecologicalImpact > other.ecologicalImpact;
        }
        
        return hDistance > other.hDistance;
    }
};

struct NodoI{
  EstadoI state;
  list<Action> sequence;

  bool operator==(const NodoI &node) const{
    return state == node.state;
  }

  bool operator<(const NodoI &node) const{
    if (state.site.f < node.state.site.f) return true;
    else if (state.site.f == node.state.site.f and state.site.c < node.state.site.c) return true;
    else if (state.site.f == node.state.site.f and state.site.c == node.state.site.c and state.site.brujula <
      node.state.site.brujula) return true;
    else if (state.site.f == node.state.site.f and state.site.c == node.state.site.c and state.site.brujula ==
      node.state.site.brujula and state.shoes < node.state.shoes) return true;
    else return false;
  }
};

struct NodoI_AEstrella {
  EstadoI state;
  list<Action> sequence;
  int g; // Cost accumulated from the start
  int f; // g + h (estimated total cost)

  bool operator>(const NodoI_AEstrella &node) const {
    return f > node.f; // Reversed so priority_queue extracts the smallest 'f'
  }
  bool operator<(const NodoI_AEstrella &node) const {
    return f < node.f; // Reversed so priority_queue extracts the smallest 'f'
  }
};

struct EstadoTuberia {
    int f, c;
    Orientacion brujula;
    int effectiveHeight;
    bool terrainAltered;
    bool pipeInstalled;
    int previousPipeHeight;

    bool operator<(const EstadoTuberia &o) const {
      if (f != o.f) return f < o.f;
      if (c != o.c) return c < o.c;
      if (brujula != o.brujula) return brujula < o.brujula;
      if (effectiveHeight != o.effectiveHeight) return effectiveHeight < o.effectiveHeight;
      if (terrainAltered != o.terrainAltered) return terrainAltered < o.terrainAltered;
      if (pipeInstalled != o.pipeInstalled) return pipeInstalled < o.pipeInstalled;
      return previousPipeHeight < o.previousPipeHeight;
    }
};

// Your state remains unchanged (f, c, heading)
// (Assuming EstadoI is already defined with its f, c, and orientation)

struct NodoAStar {
    EstadoI state; // Contains only f, c, and orientation (and shoes, if used)
    
    // Node-specific variables used for level 5
    int effectiveHeight;        // Current terrain elevation beneath this node
    bool terrainAltered;      // true if DIG or RISE has already been performed on this cell
    bool pipeInstalled;     // true if gravity has already been validated on this cell
    int previousPipeHeight;   // Pipe elevation in the previous cell

    int g, h, f_cost;
    list<Action> sequence;
    list<Paso> stepSequence;
    int energy = 0;
    int installCount = 0; // Number of times a pipe has been installed
    int ecologicalImpact;

    // The priority queue extracts the lowest f_cost
    bool operator<(const NodoAStar &other) const {
        return f_cost > other.f_cost; 
    }
};

struct ComparadorNivel4 {
    bool operator()(const NodoAStar& a, const NodoAStar& b) {
        // 1. Most important: minimize the number of pipe installations
        if (a.installCount != b.installCount) {
            return a.installCount > b.installCount;
        }
        // 2. When installation counts are equal, prefer the node with the LOWEST ecological impact
        // This leaves more room to maneuver if the terrain becomes difficult.
        if (a.ecologicalImpact != b.ecologicalImpact) {
            return a.ecologicalImpact > b.ecologicalImpact;
        }
        // 3. Final tie-breaker: energy
        return a.energy > b.energy;
    }
};

// Auxiliary structure used only for the explored (closed) set
// Prevents valid nodes at the same cell but with different elevations from being discarded
struct ClaveExplorados {
    int f, c;
    Orientacion brujula;
    int effectiveHeight;
    bool terrainAltered;
    bool pipeInstalled;
    int previousPipeHeight;

    bool operator<(const ClaveExplorados &o) const {
        if (f != o.f) return f < o.f;
        if (c != o.c) return c < o.c;
        if (brujula != o.brujula) return brujula < o.brujula;
        if (effectiveHeight != o.effectiveHeight) return effectiveHeight < o.effectiveHeight;
        if (terrainAltered != o.terrainAltered) return terrainAltered < o.terrainAltered;
        if (pipeInstalled != o.pipeInstalled) return pipeInstalled < o.pipeInstalled;
        return previousPipeHeight < o.previousPipeHeight;
    }
};

class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORS
  // =========================================================================
  //Add a potential algorithm inversely proportional to difficulty
  //time map, incremented at every time step
  //once it has enough energy, it should avoid walking through areas that require too much energy
  /**
   * @brief Constructor for levels 0, 1, and 6 (without a complete map)
   * @param size Map size (initialized later if 0)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
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
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> terrainMap, 
                         std::vector<std::vector<unsigned char>> elevationMap): 
                         Comportamiento(terrainMap, elevationMap) {
    // Initialize state variables
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &behavior)
      : Comportamiento(behavior) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Main agent decision loop.
   * Read the sensors and choose the next action.
   * 
   * USAGE EXAMPLE:
   * Action action = think(sensors);
   * return action; // The engine will execute this action
   */
  Action think(Sensores sensors);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // STUDENT IMPLEMENTATION AREA
  // =========================================================================

  // Level-specific functions (to be implemented by the student)
  
  EstadoI applyEngineerAction(Action action, const EstadoI & state, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap);

  bool isJumpViable(EstadoI next, const vector<vector<unsigned char>> &terrain);

  bool isTechnicianJumpAccessible(const EstadoI &state, const vector<vector<unsigned char>> &terrain, const
  vector<vector<unsigned char>> &heightMap);

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
   * @param hasShoes Indicates whether the agent has the shoes.
   * @return 'P' if the cell is not viable, or the terrain type otherwise.
   */
  char isViable(char collisionCell, char heightCell, int heightDifference, bool hasShoes);

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
   * @param hasShoes indicates whether the agent has the shoes
   * @return 'P' if inaccessible due to elevation, or the cell type otherwise
   */
  char isHeightViable(char cell, int difference, bool hasShoes);

  /**
   * @brief Determine the best option among the three cells ahead.
   * @param i Terrain at surface position 1 (45 degrees left)
   * @param c Terrain at surface position 2 (directly ahead)
   * @param d Terrain at surface position 3 (45 degrees right)
   * @param hasShoes indicates whether the agent has the shoes
   * @param current Current agent state
   * @return 2 for WALK, 1 for TURN_SL, and 3 for TURN_SR. 0 if nothing is interesting.
   */
  int chooseInterestingCellLevel0(char i, char c, char d, bool hasShoes, const ubicacion &current);

  int chooseInterestingCellLevel1(char i, char c, char d, bool hasShoes, const ubicacion &current);

  /**
   * @brief Implementation of level 0.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel0(Sensores sensors);
  
  /**
   * @brief Implementation of level 1.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel1(Sensores sensors);
  
  /**
   * @brief Implementation of level 2.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */ 
  Action engineerBehaviorLevel2(Sensores sensors);
  
  list<Action> breadthFirstSearchV2(const EstadoI &start, const EstadoI &goal,
                    const vector<vector<unsigned char>> &terrain,
                    const vector<vector<unsigned char>> &heightMap);

  /**
   * @brief Implementation of level 3.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel3(Sensores sensors);
  
  /**
   * @brief Implementation of level 4.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel4(Sensores sensors);

  int calculateHeuristic(int currentRow, int currentColumn, int goalRow, int goalColumn);

  pair<list<Paso>, list<Action>> calculatePipePlanActions(int minimumInstallCount, int maxEcologicalImpact, const pair<int, int> &destination, const vector<vector<unsigned char>> &terrainTypeMap, const vector<vector<unsigned char>> &terrainHeightMap, Sensores sensors);

  list<pair<int, int>> findSortedBases(const vector<vector<unsigned char>> &map, const ubicacion &belkanita);

  //list<Action> aStarBelkanitaToBase(const EstadoI &start, const pair<int, int> &destination, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap);

  int baseCost(Action action, unsigned char currentTerrain);
  
  list<Paso> aStarBelkanitaToBase(const EstadoI &start, const pair<int, int> &destination, const vector<vector<unsigned char>> &terrainMap, const vector<vector<unsigned char>> &elevationMap);

  list<Paso> breadthBelkanitaToBase(const EstadoI &start, const pair<int, int> &destination, const vector<vector<unsigned char>> &terrainMap, const vector<vector<unsigned char>> &elevationMap, int installLimit, int maxEcologicalImpact);

  /**
   * @brief Implementation of level 5.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel5(Sensores sensors);
  
  /**
   * @brief Implementation of level 6.
   * @param sensors Current agent sensor data.
   * @return Action to perform.
   */
  Action engineerBehaviorLevel6(Sensores sensors);

  pair<int, int> findUnconnectedPipe(const vector<vector<unsigned char>> &terrain);

  Action reactiveAStarLevel6(const EstadoI &start, const EstadoI &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap);
protected:
  // =========================================================================
  // PROVIDED FUNCTIONS
  // =========================================================================

  /**
   * @brief Update the internal map using sensor data.
   * IMPORTANT: This function is already implemented. It updates the terrain and elevation maps
   * using data from all 16 sensors (the current cell and 15 southrounding cells).
   */
  void updateMap(Sensores sensors);

  /**
   * @brief Check whether a cell is traversable.
   * @param f Cell row.
   * @param c Cell column.
   * @param hasShoes Indicates whether the agent has the shoes.
   * @return true if the cell is traversable (not a wall or cliff).
   */
  bool isTraversableCellLevel0(int f, int c, bool hasShoes);

  /**
   * @brief Check whether the cell ahead is accessible based on elevation difference.
   * RULES: Maximum elevation change is 1 without shoes and 2 with shoes.
   * @param current Current agent state (row, column, orientation).
   * @return true if the elevation difference with the cell ahead is acceptable.
   */
  bool isHeightAccessible(const ubicacion &current, bool hasShoes);

  /**
   * @brief Return the position (row, column) of the cell directly ahead of the agent.
   * @param current Current agent state (row, column, orientation).
   * @return State containing the row and column of the cell ahead.
   */
  ubicacion cellAhead(const ubicacion &current) const;

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

  /**
 * @brief Convert a pipe plan into the list of cells used
 *        by the visualization system.
 * @param state    Starting state (not used directly).
 * @param plan  List of steps in the pipe plan.
 */
  void visualizePipeNetwork(const list<Paso> &plan);



private:
  // =========================================================================
  // STATE VARIABLES (MAY BE EXTENDED BY THE STUDENT)
  // =========================================================================

  Action last_action;
  bool hasShoes = false;
  int left45TurnCount;

  vector<vector<int>> timeMap;
  int currentInstant = 1;

  list<Action> actionPlan;
  list<Paso> pipePlan;
  bool hasPlan;

  bool atBelkanita = false;
  bool waitingForTechnician = false;

  bool restoringOrientation = false;
  bool shouldCallTechnician = false;
  bool preparingInstall = false;
  Orientacion originalOrientation;
  bool firstCallMade = false;
  bool calledBeforeWalk = false;
  int timeSinceWalk = -1;
  int patienceCounter = 0;
  int walkOrientation = 0;

  EngineerPhase currentEngineerPhase = SEARCH_BELKANITA;
  int targetPipeRow = -1;
  int targetPipeColumn = -1;

  set<pair<int, int>> discardedPipes; 
  bool constructionFailed = false;
  bool constructionFinished = false;
  bool firstTimeLevel6 = true;
};

#endif
