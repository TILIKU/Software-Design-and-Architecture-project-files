#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// STUDENT IMPLEMENTATION AREA
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensors) {
  Action action = IDLE;
  currentInstant++;

  cout << "Instante: " << currentInstant << " | Tecnico piensa..." << endl;
  // Choose the agent's behavior based on its level
  switch (sensors.nivel) {
    case 0: action = technicianBehaviorLevel0(sensors); break;
    case 1: action = technicianBehaviorLevel1(sensors); break;
    case 2: action = technicianBehaviorLevel2(sensors); break;
    case 3: action = technicianBehaviorLevel3(sensors); break;
    //case 3: action = technicianBehaviorLevelE(sensors); break;
    case 4: action = technicianBehaviorLevel4(sensors); break;
    case 5: action = technicianBehaviorLevel5(sensors); break;
    case 6: action = technicianBehaviorLevel6(sensors); break;
  }

  return action;
}

// Technician levels
/**
 * @brief Update the time map with the current time step at the given position.
 * @param f Row of the current cell.
 * @param c Column of the current cell.
 */
void ComportamientoTecnico::updateTimeMap(int f, int c) {
  timeMap[f][c] = currentInstant;
}

char ComportamientoTecnico::isViable(char collisionCell, char heightCell, int heightDifference){
  if (isCollisionViable(collisionCell))
    return isHeightViable(heightCell, heightDifference);
  else
    return 'P';
}

bool ComportamientoTecnico::isCollisionViable(char cell){
  if (cell != 'i')
    return true;
  else
    return false;
}

char ComportamientoTecnico::isHeightViable(char cell, int difference){
  if (abs(difference) <= 1)
    return cell;
  else
    return 'P';
}

int ComportamientoTecnico::exitLoop(){
  if (last_action == TURN_SL)
    return 1;
  else if (last_action == TURN_SR)
    return 3;
  else
    return 0;
}

/**
 * @brief Determine the best option among the three cells ahead.
 * @param i Terrain at surface position 1 (45 degrees left)
 * @param c Terrain at surface position 2 (directly ahead)
 * @param d Terrain at surface position 3 (45 degrees right)
 * @param hasShoes indicates whether the agent has the shoes
 * @return 2 for WALK, 1 for TURN_SL, and 3 for TURN_SR. 0 if nothing is interesting.
 */
int ComportamientoTecnico::chooseInterestingCellLevel0(char i, char c, char d, Sensores sensors){
  if (c == 'U') return 2;
  else if (i == 'U') return 1;
  else if (d == 'U') return 3;

  if (c == 'D')
    c = 'C';
  if (i == 'D')
    i = 'C';
  if (d == 'D')
    d = 'C';

  ubicacion frontPosition = cellAhead({sensors.posF, sensors.posC, sensors.rumbo});
  ubicacion left45Position = cellAhead({sensors.posF, sensors.posC, (Orientacion)((sensors.rumbo + 7) % 8)});
  ubicacion right45Position = cellAhead({sensors.posF, sensors.posC, (Orientacion)((sensors.rumbo + 1) % 8)});

  int best = 0;
  int bestValue = 9999;

  // Center (highest priority)
  if (c == 'C') {
      int v = timeMap[frontPosition.f][frontPosition.c];
      if (v < bestValue) {
          bestValue = v;
          best = 2;
      }
  }

  // Right
  if (d == 'C') {
      int v = timeMap[right45Position.f][right45Position.c];
      if (v < bestValue) {
          bestValue = v;
          best = 3;
      }
  }

  // Left
  if (i == 'C') {
      int v = timeMap[left45Position.f][left45Position.c];
      if (v < bestValue) {
          bestValue = v;
          best = 1;
      }
  }

  if (aleatorio(100) < 5)
    return exitLoop();

  return best;
}

int ComportamientoTecnico::chooseInterestingCellLevel1(char i, char c, char d, Sensores sensors){
  if (c == 'D' || c == 'S')
    c = 'C';
  if (i == 'D' || i == 'S')
    i = 'C';
  if (d == 'D' || d == 'S')
    d = 'C';

  ubicacion frontPosition = cellAhead({sensors.posF, sensors.posC, sensors.rumbo});
  ubicacion left45Position = cellAhead({sensors.posF, sensors.posC, (Orientacion)((sensors.rumbo + 7) % 8)});
  ubicacion right45Position = cellAhead({sensors.posF, sensors.posC, (Orientacion)((sensors.rumbo + 1) % 8)});

  int best = 0;
  int bestValue = 9999;

  if (c == 'C') {
      int v = timeMap[frontPosition.f][frontPosition.c];
      if (v < bestValue) {
          bestValue = v;
          best = 2;
      }
  }

  if (d == 'C') {
      int v = timeMap[right45Position.f][right45Position.c];
      if (v < bestValue) {
          bestValue = v;
          best = 3;
      }
  }

  if (i == 'C') {
      int v = timeMap[left45Position.f][left45Position.c];
      if (v < bestValue) {
          bestValue = v;
          best = 1;
      }
  }

  if (aleatorio(100) < 5)
    return exitLoop();

  return best;
}

Action ComportamientoTecnico::technicianBehaviorLevel0(Sensores sensors) {
  Action action = IDLE;

  updateMap(sensors);
  updateTimeMap(sensors.posF, sensors.posC);

  // Update state variables
  if (sensors.superficie[0] == 'D') hasShoes = true;

  if (sensors.superficie[0] == 'U' && sensors.superficie[2] != 'U' && sensors.superficie[3] != 'U' && sensors.superficie[1] != 'U'){ // Reached a 'U'.
    if (aleatorio(100) > 20){
      return IDLE;
    } else {
      return TURN_SL;
    }
  } else if (sensors.superficie[0] == 'U' && sensors.superficie[2] == 'U'){
    return WALK;
  } else if (sensors.superficie[0] == 'U' && sensors.superficie[1] == 'U'){
    return TURN_SL;
  } else if (sensors.superficie[0] == 'U' && sensors.superficie[3] == 'U'){
    return TURN_SR;
  }
  char i = isViable(sensors.agentes[1], sensors.superficie[1], sensors.cota[1] - sensors.cota[0]);
  char c = isViable(sensors.agentes[2], sensors.superficie[2], sensors.cota[2] - sensors.cota[0]);
  char d = isViable(sensors.agentes[3], sensors.superficie[3], sensors.cota[3] - sensors.cota[0]);


  int position = chooseInterestingCellLevel0(
    i,
    c,
    d,
    sensors
  );


  switch (position)
  {
    case 2:
      action = WALK;
      break;
    case 1:
      action = TURN_SL;
      break;
    case 3:
      action = TURN_SR;
      break;
    default:
      action = TURN_SL;
      break;
  }

  last_action = action;
  return action;
}

/**
 * @brief Check whether a cell is a traversable path.
 * @param c Character representing the surface type.
 * @return true if it is a path ('C'), shoes ('D'), or goal ('U').
 */
bool ComportamientoTecnico::isPath(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * @brief Reactive behavior of the technician for level 1.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoTecnico::technicianBehaviorLevel1(Sensores sensors) {
  Action action = IDLE;

  // Follow a path until a waste treatment plant is found.
  // Write the vision sensor values to the maps.
  updateMap(sensors);
  updateTimeMap(sensors.posF, sensors.posC);

  if (sensors.superficie[0] == 'D') hasShoes = true;

  if (sensors.superficie[0] == 'U'){
      return IDLE;
  }

  char i = isViable(sensors.agentes[1], sensors.superficie[1], sensors.cota[1] - sensors.cota[0]);
  char c = isViable(sensors.agentes[2], sensors.superficie[2], sensors.cota[2] - sensors.cota[0]);
  char d = isViable(sensors.agentes[3], sensors.superficie[3], sensors.cota[3] - sensors.cota[0]);


  int position = chooseInterestingCellLevel1(
    i,
    c,
    d,
    sensors
  );


  switch (position)
  {
    case 2:
      action = WALK;
      break;
    case 1:
      action = TURN_SL;
      break;
    case 3:
      action = TURN_SR;
      break;
    default:
      action = TURN_SL;
      break;
  }

  last_action = action;
  return action;
}

/**
 * @brief Behavior of the technician for level 2.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoTecnico::technicianBehaviorLevel2(Sensores sensors) {
  return IDLE;
}

int ComportamientoTecnico::heuristic(const EstadoT &current, const EstadoT &goal) {
  return std::max(abs(current.site.f - goal.site.f), abs(current.site.c - goal.site.c));
}

int ComportamientoTecnico::baseCost(Action action, unsigned char currentTerrain) {
    int cost = 0;

    if (action == WALK) {
        switch (currentTerrain) {
            case 'A': cost = 60; break; // Fill in using your tables
            case 'H': cost = 6; break; 
            case 'S': cost = 3; break;
            default: cost = 1; break;
        }
    } else if (action == TURN_SL || action == TURN_SR) {
        // When turning, we stay in the same cell, so the cost depends on OUR CURRENT LOCATION
        switch (currentTerrain) {
            case 'A': cost = 5; break;
            case 'H': cost = 2; break;
            default: cost = 1; break;
        }
    }
    return cost;
}

int ComportamientoTecnico::heightCostModifier(int currentHeight, int destinationHeight, int baseCostValue,  unsigned char currentTerrain) {
  int difference = destinationHeight - currentHeight;
  if (difference > 0) {
    switch (currentTerrain) {
      case 'A': baseCostValue += 5; break;
      case 'H': baseCostValue += 5; break;
      case 'S': baseCostValue += 5; break;
      default: baseCostValue += 0; break;
    }
  } else if (difference < 0) {
    switch (currentTerrain) {
      case 'A': baseCostValue -= 2; break;
      case 'H': baseCostValue -= 2; break;
      case 'S': baseCostValue -= 2; break;
      default: baseCostValue += 0; break;
    }
  }else{
    baseCostValue += 0;
  }
  return baseCostValue;
}

list<Action> ComportamientoTecnico::aStarSearch(const EstadoT &start, const EstadoT &goal,
                         const vector<vector<unsigned char>> &terrain,
                         const vector<vector<unsigned char>> &heightMap,
                         int obsF, int obsC){
  NodoTCoste currentNode;
  priority_queue<NodoTCoste, vector<NodoTCoste>, ComparaCostes> frontier;
  
  map<EstadoT, int> explored; 
  list<Action> path;

  currentNode.state = start;
  currentNode.cost = 0; 
  currentNode.heuristicEstimate = heuristic(start, goal);
  frontier.push(currentNode);

  while (!frontier.empty()) {
    currentNode = frontier.top();
    frontier.pop();

    if (currentNode.state.site.f == goal.site.f && currentNode.state.site.c == goal.site.c) {
      return currentNode.sequence;
    }

    if (explored.count(currentNode.state) && explored[currentNode.state] <= currentNode.cost) {
        continue;
    }

    explored[currentNode.state] = currentNode.cost;

    Action acciones[3] = {WALK, TURN_SL, TURN_SR};

    for (int i = 0; i < 3; i++) {
      Action action = acciones[i];
      NodoTCoste child = currentNode;
      
      child.state = applyTechnicianAction(action, currentNode.state, terrain, heightMap);

      if (terrain[child.state.site.f][child.state.site.c] == 'D') {
        child.state.shoes = true;
      }

      if (!(child.state == currentNode.state)) {
        
        int f = child.state.site.f;
        int c = child.state.site.c;
        if (f < 0 || f >= terrain.size() || c < 0 || c >= terrain[0].size()) {
            continue; 
        }

        if (f == obsF && c == obsC) {
            continue;
        }
        child.sequence.push_back(action);
        
        unsigned char currentTerrain = terrain[currentNode.state.site.f][currentNode.state.site.c];
        unsigned char destinationTerrain = terrain[f][c];
        
        int currentHeight = heightMap[currentNode.state.site.f][currentNode.state.site.c];
        int destinationHeight = heightMap[f][c];

        int actionCost;
        if (action == WALK) {
            actionCost = baseCost(action, destinationTerrain); 
        } else {
            actionCost = baseCost(action, currentTerrain);
        }
        
        if (action == WALK) {
          actionCost = heightCostModifier(currentHeight, destinationHeight, actionCost, currentTerrain);
        }

        if (actionCost < 1) actionCost = 1; 

        child.cost += actionCost; 
        child.heuristicEstimate = heuristic(child.state, goal);

        if (explored.find(child.state) == explored.end() || child.cost < explored[child.state]) {
            frontier.push(child);
        }
      }
    }
  } 
  return path;
}

/**
 * @brief Behavior of the technician for level 3.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoTecnico::technicianBehaviorLevel3(Sensores sensors) {
  Action action = IDLE;

  if (sensors.superficie[0] == 'D') hasShoes = true;

  if (!hasPlan){
    EstadoT start, fin;
    start.site.f = sensors.posF;
    start.site.c = sensors.posC;
    start.site.brujula = sensors.rumbo;
    start.shoes = hasShoes;
    fin.site.f = sensors.BelPosF;
    fin.site.c = sensors.BelPosC;
    plan = aStarSearch(start, fin, mapaResultado, mapaCotas);
    visualizePlan(start.site,plan);
    hasPlan = plan.size() != 0 ;
  }
  if (hasPlan and plan.size()>0){
    action = plan.front();
    plan.pop_front();
  }
  if (plan.size()== 0){
    hasPlan = false;
  }
  return action;
}

bool ComportamientoTecnico::isTechnicianCellAccessible(const EstadoT &state, const vector<vector<unsigned char>> &terrain, const
  vector<vector<unsigned char>> &heightMap){
  EstadoT next = nextTechnicianCell(state);

  if (next.site.f < 0 || next.site.f >= terrain.size() || 
    next.site.c < 0 || next.site.c >= terrain[0].size()) {
    return false;   
  }

  bool check1 = false, check2 = false, check3 = false;
  check1 = terrain[next.site.f][next.site.c] != 'P' and terrain[next.site.f][next.site.c] != 'M';
  check2 = terrain[next.site.f][next.site.c] != 'B' or (terrain[next.site.f][next.site.c] == 'B' and state.shoes);
  if (terrain[next.site.f][next.site.c] == '?') {
      check3 = true; 
  } else {
      check3 = abs(heightMap[next.site.f][next.site.c] - heightMap[state.site.f][state.site.c]) <= 1;
  }
  return check1 and check2 and check3;
}

EstadoT ComportamientoTecnico::nextTechnicianCell(const EstadoT &state){
  EstadoT next = state;
  switch (state.site.brujula)
  {
  case norte:
    next.site.f = state.site.f - 1;
    break;
  case noreste:
    next.site.f = state.site.f - 1;
    next.site.c = state.site.c + 1;
    break;
  case este:
    next.site.c = state.site.c + 1;
    break;
  case sureste:
    next.site.f = state.site.f + 1;
    next.site.c = state.site.c + 1;
    break;
  case sur:
    next.site.f = state.site.f + 1;
    break;
  case suroeste:
    next.site.f = state.site.f + 1;
    next.site.c = state.site.c - 1;
    break;
  case oeast:
    next.site.c = state.site.c - 1;
    break;
  case noroeast:
    next.site.f = state.site.f - 1;
    next.site.c = state.site.c - 1;
  }
  return next;
}

EstadoT ComportamientoTecnico::applyTechnicianAction(Action action, const EstadoT & state, const vector<vector<unsigned char>> &terrain, const
  vector<vector<unsigned char>> &heightMap){
  EstadoT next = state;
  switch(action){
  case WALK:
    if (isTechnicianCellAccessible(state,terrain,heightMap)){
      next = nextTechnicianCell(state);
    }
    break;
  case TURN_SR:
    next.site.brujula = (Orientacion) ((next.site.brujula+1)%8);
    break;
  case TURN_SL:
    next.site.brujula = (Orientacion) ((next.site.brujula+7)%8);
    break;
  }

  if (terrain[next.site.f][next.site.c] == 'D') {
      next.shoes = true;
  }

  return next;
}

/**
 * @brief Behavior of the technician for level 4.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoTecnico::technicianBehaviorLevel4(Sensores sensors) {
  return IDLE;
}

/**
 * @brief Behavior of the technician for level 5.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */

Action ComportamientoTecnico::technicianBehaviorLevel5(Sensores sensors) {
  if (sensors.superficie[0] == 'D') hasShoes = true;

  if (timeSinceWalk >= 0) timeSinceWalk++;

  // =======================================================
  // PHASE 1: RECEIVE THE CALL
  // =======================================================
  if (sensors.venpaca) {
    destinationRow = sensors.GotoF;
    destinationColumn = sensors.GotoC;

    EstadoT start, fin;
    start.site.f = sensors.posF; start.site.c = sensors.posC;
    start.site.brujula = sensors.rumbo; start.shoes = hasShoes;
    fin.site.f = destinationRow; fin.site.c = destinationColumn;

    plan = aStarSearch(start, fin, mapaResultado, mapaCotas);
    hasPlan = !plan.empty();
    hasReceivedCall = true;
    hasArrived = false; 
  }

  // =======================================================
  // PHASE 2: MOVEMENT
  // =======================================================
  if (hasPlan && !plan.empty()) {
    Action action = plan.front();
    
    // --- NEW REACTIVE COLLISION-AVOIDANCE SYSTEM ---
    if (action == WALK && sensors.agentes[2] == 'i') {
        int obstacleRow = sensors.posF;
        int obstacleColumn = sensors.posC;
        switch(sensors.rumbo) {
            case 0: obstacleRow--; break; // North
            case 1: obstacleRow--; obstacleColumn++; break; // Northeast
            case 2: obstacleColumn++; break; // East
            case 3: obstacleRow++; obstacleColumn++; break; // Southeast
            case 4: obstacleRow++; break; // South
            case 5: obstacleRow++; obstacleColumn--; break; // Southoeast
            case 6: obstacleColumn--; break; // West
            case 7: obstacleRow--; obstacleColumn--; break; // Northwest
        }

        if (obstacleRow == destinationRow && obstacleColumn == destinationColumn) {
            return IDLE;
        }
        EstadoT start, fin;
        start.site.f = sensors.posF; start.site.c = sensors.posC;
        start.site.brujula = sensors.rumbo; start.shoes = hasShoes;
        fin.site.f = destinationRow; fin.site.c = destinationColumn;

        plan = aStarSearch(start, fin, mapaResultado, mapaCotas, obstacleRow, obstacleColumn);
        hasPlan = !plan.empty();
        
        return IDLE;
    }

    plan.pop_front();
    if (action == WALK) timeSinceWalk = 0;

    if (plan.empty()) {
        hasPlan = false;
        hasArrived = true;
    }
    return action;
  }

  if (hasArrived) {
      if (sensors.enfrente) {
        cout << "Ingeniero hace INSTALL" << endl;  
        return INSTALL;
      } 
      else {
          if (sensors.agentes[1] == 'i') return TURN_SL;
          else if (sensors.agentes[3] == 'i') return TURN_SR;
          else if (sensors.agentes[2] == 'i') return IDLE;
          else {
              bool ing_en_vision = false;
              for (int i = 4; i < 16; i++) {
                  if (sensors.agentes[i] == 'i') { ing_en_vision = true; break; }
              }
              return ing_en_vision ? IDLE : TURN_SR; 
          }
      }
  }

  return IDLE;
}

Action ComportamientoTecnico::engineerBehaviorLevel2(Sensores sensors)
{
  Action action = IDLE;
  if (!hasPlan){
  // Call the search method
    EstadoT start, fin;
    start.site.f = sensors.posF;
    start.site.c = sensors.posC;
    start.site.brujula = sensors.rumbo;
    start.shoes = hasShoes;
    fin.site.f = sensors.GotoF;
    fin.site.c = sensors.GotoC;
    plan = breadthFirstSearchV2(start, fin, mapaResultado, mapaCotas);
    visualizePlan(start.site,plan);
    hasPlan = plan.size() != 0 ;
  }
  if (hasPlan and plan.size()>0){
    action = plan.front();
    plan.pop_front();
  }
  if (plan.size()== 0){
    hasPlan = false;
  }
  return action;
}

/**
 * @brief Behavior of the technician for level 6.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoTecnico::technicianBehaviorLevel6(Sensores sensors) {
    updateMap(sensors);
    Action action = IDLE;

    if (sensors.venpaca && currentTechnicianPhase == LEVEL_6_SURVIVAL_MODE) {
        currentTechnicianPhase = GO_TO_ENGINEER;
    }

    switch (currentTechnicianPhase) {

        case LEVEL_6_SURVIVAL_MODE:
            if (sensors.energy > 1000) {
                return technicianBehaviorLevel1(sensors);
            }
            return IDLE;

        case GO_TO_ENGINEER: {

            if (sensors.posF == belkanitaGoal.site.f && sensors.posC == belkanitaGoal.site.c) {
              if (sensors.enfrente){
                currentTechnicianPhase = RUN_LEVEL_5_TECHNICIAN;
                return INSTALL; 
              } else {
                return TURN_SL;
              }
            }
            
            if(sensors.agentes[2] != 'i' && sensors.agentes[1] != 'i' && sensors.agentes[3] != 'i'){
              localMap = mapaResultado;
            }

            if (sensors.choque) {
                cout << "¡TÉCNICO! Choque detectado. Calculando cell obstáculo..." << endl;
                int aheadRow = sensors.posF;
                int aheadColumn = sensors.posC;

                switch (sensors.rumbo) {
                    case 0: aheadRow--; break;      // North
                    case 1: aheadRow--; aheadColumn++; break; // Northeast
                    case 2: aheadColumn++; break;      // East
                    case 3: aheadRow++; aheadColumn++; break; // Southeast
                    case 4: aheadRow++; break;      // South
                    case 5: aheadRow++; aheadColumn--; break; // Southoeast
                    case 6: aheadColumn--; break;      // West
                    case 7: aheadRow--; aheadColumn--; break; // Northwest
                }

                if (aheadRow >= 0 && aheadRow < localMap.size() && aheadColumn >= 0 && aheadColumn < localMap[0].size()) {
                    cout << "Bloqueando temporalmente cell (" << aheadRow << ", " << aheadColumn << ") para el A*" << endl;
                    localMap[aheadRow][aheadColumn] = 'M'; // Treat it as a wall ('M').
                }
            }

            EstadoT current;
            current.site.f = sensors.posF;
            current.site.c = sensors.posC;
            current.site.brujula = sensors.rumbo;
            
            if (sensors.venpaca){
              belkanitaGoal.site.f = sensors.GotoF;
              belkanitaGoal.site.c = sensors.GotoC;
            }

            return reactiveAStarLevel6(current, belkanitaGoal, localMap, mapaCotas);
        }

        case RUN_LEVEL_5_TECHNICIAN:
            action = technicianBehaviorLevel5(sensors);
            
            return action;
    }

    return IDLE;
}

int ComportamientoTecnico::calculateHeuristic(int currentRow, int currentColumn, int goalRow, int goalColumn) {
    return abs(currentRow - goalRow) + abs(currentColumn - goalColumn);
}

Action ComportamientoTecnico::reactiveAStarLevel6(const EstadoT &start, const EstadoT &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap) {
    
    priority_queue<NodoT_AEstrella, vector<NodoT_AEstrella>, greater<NodoT_AEstrella>> frontier;
    set<EstadoT> explored;

    NodoT_AEstrella startNode;
    startNode.state = start;
    startNode.g = 0;
    startNode.f = calculateHeuristic(start.site.f, start.site.c, goal.site.f, goal.site.c);
    
    frontier.push(startNode);

    while (!frontier.empty()) {
        NodoT_AEstrella currentNode = frontier.top();
        frontier.pop();

        if (currentNode.state.site.f == goal.site.f && currentNode.state.site.c == goal.site.c) {
            if (!currentNode.sequence.empty()) {
                return currentNode.sequence.front();
            }
            return IDLE;
        }

        if (explored.find(currentNode.state) != explored.end()) {
            continue;
        }
        explored.insert(currentNode.state);

        Action acciones[3] = {WALK, TURN_SL, TURN_SR};

        for (int i = 0; i < 3; i++) {
            Action action = acciones[i];
            NodoT_AEstrella child = currentNode;
            
            child.state = applyTechnicianAction(action, currentNode.state, terrain, heightMap);

            if (!(child.state == currentNode.state)) {
                child.g = currentNode.g + 1;
                child.f = child.g + calculateHeuristic(child.state.site.f, child.state.site.c, goal.site.f, goal.site.c);
                
                child.sequence.push_back(action);

                if (explored.find(child.state) == explored.end()) {
                    frontier.push(child);
                }
            }
        }
    }
    return TURN_SR; 
}

bool findNode (const NodoT & state, const list<NodoT> &nodeList){
  auto it = nodeList.begin();
  while (it != nodeList.end() and !((*it) == state)){
    it++;
  }
  return (it != nodeList.end());
}

/**
 * @brief First attempt at breadth-first search
 * 
 * @param start Initial search state.
 * @param goal Goal search state.
 * @param terrain Matrix containing terrain information.
 * @param heightMap Matrix containing map elevation data.
 * 
 * @return Action sequence for reaching the goal state.
 * @note Returns an empty plan if no valid plan can be found.
 */
/**
 * @brief Second attempt at breadth-first search
 * 
 * @param start Initial search state.
 * @param goal Goal search state.
 * @param terrain Matrix containing terrain information.
 * @param heightMap Matrix containing map elevation data.
 * 
 * @return Action sequence for reaching the goal state.
 * @note Returns an empty plan if no valid plan can be found.
 * @note Explored uses a set instead of a list
 */
list<Action> ComportamientoTecnico::breadthFirstSearchV2(const EstadoT &start, const EstadoT &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap) {
  NodoT currentNode;
  list<NodoT> frontier;
  set<NodoT> explored;
  list<Action> path;

  currentNode.state = start;
  frontier.push_back(currentNode);

  bool solutionFound = (currentNode.state.site.f == goal.site.f and currentNode.state.site.c == goal.site.c);

  while (!solutionFound and !frontier.empty()){
    frontier.pop_front();
    explored.insert(currentNode);

    if (terrain[currentNode.state.site.f][currentNode.state.site.c] == 'D'){
        currentNode.state.shoes = true;
    }

    NodoT walkChild = currentNode;
    walkChild.state = applyTechnicianAction(WALK, currentNode.state, terrain, heightMap);
    if (walkChild.state.site.f == goal.site.f and walkChild.state.site.c == goal.site.c){
      walkChild.sequence.push_back(WALK);
      currentNode = walkChild;
      solutionFound = true;
    }
    else if (explored.find(walkChild) == explored.end()){
      walkChild.sequence.push_back(WALK);
      frontier.push_back(walkChild);
    }

    if (!solutionFound){
      NodoT rightTurnChild = currentNode;
      rightTurnChild.state = applyTechnicianAction(TURN_SR, currentNode.state, terrain, heightMap);
      if (explored.find(rightTurnChild) == explored.end()){
        rightTurnChild.sequence.push_back(TURN_SR);
        frontier.push_back(rightTurnChild);
      }

      NodoT leftTurnChild = currentNode;
      leftTurnChild.state = applyTechnicianAction(TURN_SL, currentNode.state, terrain, heightMap);
      if (explored.find(leftTurnChild) == explored.end()){
        leftTurnChild.sequence.push_back(TURN_SL);
        frontier.push_back(leftTurnChild);
      }
    }

    if (!solutionFound and !frontier.empty()){
      currentNode = frontier.front();
      while (explored.find(currentNode) != explored.end() and !frontier.empty()){
        frontier.pop_front();
        currentNode = frontier.front();
      }
    }
  }

  if (solutionFound) {
      path = currentNode.sequence;
  }

  return path;
}

list<Action> buildKnightMoveSequence() {
    list<Action> sequence;
    sequence.push_back(WALK);
    sequence.push_back(WALK);
    sequence.push_back(TURN_SR);
    sequence.push_back(TURN_SR);
    sequence.push_back(WALK);
    return sequence;
}

/**
* @brief Behavior of the technician for level E.
* @param sensors Current sensor data.
* @return Action to perform.
*/
Action ComportamientoTecnico::technicianBehaviorLevelE(Sensores sensors) {
  Action action = IDLE;
  if (!hasPlan){
    EstadoT start, fin;
    start.site.f = sensors.posF;
    start.site.c = sensors.posC;
    start.site.brujula = sensors.rumbo;
    start.shoes = hasShoes;
    fin.site.f = sensors.BelPosF;
    fin.site.c = sensors.BelPosC;
    plan = breadthFirstSearchV2(start, fin, mapaResultado, mapaCotas);
    visualizePlan(start.site,plan);
    hasPlan = plan.size() != 0 ;
  }
  if (hasPlan and plan.size()>0){
    action = plan.front();
    plan.pop_front();
  }
  if (plan.size()== 0){
    hasPlan = false;
  }
  return action;
}

// =========================================================================
// PROVIDED FUNCTIONS
// =========================================================================

/**
 * @brief Update the terrain and elevation maps using sensor data.
 * @param sensors Current sensor data.
 */
void ComportamientoTecnico::updateMap(Sensores sensors) {
  mapaResultado[sensors.posF][sensors.posC] = sensors.superficie[0];
  mapaCotas[sensors.posF][sensors.posC] = sensors.cota[0];

  int position = 1;
  switch (sensors.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensors.posF - j][sensors.posC + i] = sensors.superficie[position];
          mapaCotas[sensors.posF - j][sensors.posC + i] = sensors.cota[position++];
        }
      break;
    case noreste:
      mapaResultado[sensors.posF - 1][sensors.posC] = sensors.superficie[1];
      mapaCotas[sensors.posF - 1][sensors.posC] = sensors.cota[1];
      mapaResultado[sensors.posF - 1][sensors.posC + 1] = sensors.superficie[2];
      mapaCotas[sensors.posF - 1][sensors.posC + 1] = sensors.cota[2];
      mapaResultado[sensors.posF][sensors.posC + 1] = sensors.superficie[3];
      mapaCotas[sensors.posF][sensors.posC + 1] = sensors.cota[3];
      mapaResultado[sensors.posF - 2][sensors.posC] = sensors.superficie[4];
      mapaCotas[sensors.posF - 2][sensors.posC] = sensors.cota[4];
      mapaResultado[sensors.posF - 2][sensors.posC + 1] = sensors.superficie[5];
      mapaCotas[sensors.posF - 2][sensors.posC + 1] = sensors.cota[5];
      mapaResultado[sensors.posF - 2][sensors.posC + 2] = sensors.superficie[6];
      mapaCotas[sensors.posF - 2][sensors.posC + 2] = sensors.cota[6];
      mapaResultado[sensors.posF - 1][sensors.posC + 2] = sensors.superficie[7];
      mapaCotas[sensors.posF - 1][sensors.posC + 2] = sensors.cota[7];
      mapaResultado[sensors.posF][sensors.posC + 2] = sensors.superficie[8];
      mapaCotas[sensors.posF][sensors.posC + 2] = sensors.cota[8];
      mapaResultado[sensors.posF - 3][sensors.posC] = sensors.superficie[9];
      mapaCotas[sensors.posF - 3][sensors.posC] = sensors.cota[9];
      mapaResultado[sensors.posF - 3][sensors.posC + 1] = sensors.superficie[10];
      mapaCotas[sensors.posF - 3][sensors.posC + 1] = sensors.cota[10];
      mapaResultado[sensors.posF - 3][sensors.posC + 2] = sensors.superficie[11];
      mapaCotas[sensors.posF - 3][sensors.posC + 2] = sensors.cota[11];
      mapaResultado[sensors.posF - 3][sensors.posC + 3] = sensors.superficie[12];
      mapaCotas[sensors.posF - 3][sensors.posC + 3] = sensors.cota[12];
      mapaResultado[sensors.posF - 2][sensors.posC + 3] = sensors.superficie[13];
      mapaCotas[sensors.posF - 2][sensors.posC + 3] = sensors.cota[13];
      mapaResultado[sensors.posF - 1][sensors.posC + 3] = sensors.superficie[14];
      mapaCotas[sensors.posF - 1][sensors.posC + 3] = sensors.cota[14];
      mapaResultado[sensors.posF][sensors.posC + 3] = sensors.superficie[15];
      mapaCotas[sensors.posF][sensors.posC + 3] = sensors.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensors.posF + i][sensors.posC + j] = sensors.superficie[position];
          mapaCotas[sensors.posF + i][sensors.posC + j] = sensors.cota[position++];
        }
      break;
    case sureste:
      mapaResultado[sensors.posF][sensors.posC + 1] = sensors.superficie[1];
      mapaCotas[sensors.posF][sensors.posC + 1] = sensors.cota[1];
      mapaResultado[sensors.posF + 1][sensors.posC + 1] = sensors.superficie[2];
      mapaCotas[sensors.posF + 1][sensors.posC + 1] = sensors.cota[2];
      mapaResultado[sensors.posF + 1][sensors.posC] = sensors.superficie[3];
      mapaCotas[sensors.posF + 1][sensors.posC] = sensors.cota[3];
      mapaResultado[sensors.posF][sensors.posC + 2] = sensors.superficie[4];
      mapaCotas[sensors.posF][sensors.posC + 2] = sensors.cota[4];
      mapaResultado[sensors.posF + 1][sensors.posC + 2] = sensors.superficie[5];
      mapaCotas[sensors.posF + 1][sensors.posC + 2] = sensors.cota[5];
      mapaResultado[sensors.posF + 2][sensors.posC + 2] = sensors.superficie[6];
      mapaCotas[sensors.posF + 2][sensors.posC + 2] = sensors.cota[6];
      mapaResultado[sensors.posF + 2][sensors.posC + 1] = sensors.superficie[7];
      mapaCotas[sensors.posF + 2][sensors.posC + 1] = sensors.cota[7];
      mapaResultado[sensors.posF + 2][sensors.posC] = sensors.superficie[8];
      mapaCotas[sensors.posF + 2][sensors.posC] = sensors.cota[8];
      mapaResultado[sensors.posF][sensors.posC + 3] = sensors.superficie[9];
      mapaCotas[sensors.posF][sensors.posC + 3] = sensors.cota[9];
      mapaResultado[sensors.posF + 1][sensors.posC + 3] = sensors.superficie[10];
      mapaCotas[sensors.posF + 1][sensors.posC + 3] = sensors.cota[10];
      mapaResultado[sensors.posF + 2][sensors.posC + 3] = sensors.superficie[11];
      mapaCotas[sensors.posF + 2][sensors.posC + 3] = sensors.cota[11];
      mapaResultado[sensors.posF + 3][sensors.posC + 3] = sensors.superficie[12];
      mapaCotas[sensors.posF + 3][sensors.posC + 3] = sensors.cota[12];
      mapaResultado[sensors.posF + 3][sensors.posC + 2] = sensors.superficie[13];
      mapaCotas[sensors.posF + 3][sensors.posC + 2] = sensors.cota[13];
      mapaResultado[sensors.posF + 3][sensors.posC + 1] = sensors.superficie[14];
      mapaCotas[sensors.posF + 3][sensors.posC + 1] = sensors.cota[14];
      mapaResultado[sensors.posF + 3][sensors.posC] = sensors.superficie[15];
      mapaCotas[sensors.posF + 3][sensors.posC] = sensors.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensors.posF + j][sensors.posC - i] = sensors.superficie[position];
          mapaCotas[sensors.posF + j][sensors.posC - i] = sensors.cota[position++];
        }
      break;
    case suroeste:
      mapaResultado[sensors.posF + 1][sensors.posC] = sensors.superficie[1];
      mapaCotas[sensors.posF + 1][sensors.posC] = sensors.cota[1];
      mapaResultado[sensors.posF + 1][sensors.posC - 1] = sensors.superficie[2];
      mapaCotas[sensors.posF + 1][sensors.posC - 1] = sensors.cota[2];
      mapaResultado[sensors.posF][sensors.posC - 1] = sensors.superficie[3];
      mapaCotas[sensors.posF][sensors.posC - 1] = sensors.cota[3];
      mapaResultado[sensors.posF + 2][sensors.posC] = sensors.superficie[4];
      mapaCotas[sensors.posF + 2][sensors.posC] = sensors.cota[4];
      mapaResultado[sensors.posF + 2][sensors.posC - 1] = sensors.superficie[5];
      mapaCotas[sensors.posF + 2][sensors.posC - 1] = sensors.cota[5];
      mapaResultado[sensors.posF + 2][sensors.posC - 2] = sensors.superficie[6];
      mapaCotas[sensors.posF + 2][sensors.posC - 2] = sensors.cota[6];
      mapaResultado[sensors.posF + 1][sensors.posC - 2] = sensors.superficie[7];
      mapaCotas[sensors.posF + 1][sensors.posC - 2] = sensors.cota[7];
      mapaResultado[sensors.posF][sensors.posC - 2] = sensors.superficie[8];
      mapaCotas[sensors.posF][sensors.posC - 2] = sensors.cota[8];
      mapaResultado[sensors.posF + 3][sensors.posC] = sensors.superficie[9];
      mapaCotas[sensors.posF + 3][sensors.posC] = sensors.cota[9];
      mapaResultado[sensors.posF + 3][sensors.posC - 1] = sensors.superficie[10];
      mapaCotas[sensors.posF + 3][sensors.posC - 1] = sensors.cota[10];
      mapaResultado[sensors.posF + 3][sensors.posC - 2] = sensors.superficie[11];
      mapaCotas[sensors.posF + 3][sensors.posC - 2] = sensors.cota[11];
      mapaResultado[sensors.posF + 3][sensors.posC - 3] = sensors.superficie[12];
      mapaCotas[sensors.posF + 3][sensors.posC - 3] = sensors.cota[12];
      mapaResultado[sensors.posF + 2][sensors.posC - 3] = sensors.superficie[13];
      mapaCotas[sensors.posF + 2][sensors.posC - 3] = sensors.cota[13];
      mapaResultado[sensors.posF + 1][sensors.posC - 3] = sensors.superficie[14];
      mapaCotas[sensors.posF + 1][sensors.posC - 3] = sensors.cota[14];
      mapaResultado[sensors.posF][sensors.posC - 3] = sensors.superficie[15];
      mapaCotas[sensors.posF][sensors.posC - 3] = sensors.cota[15];
      break;
    case oeast:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensors.posF - i][sensors.posC - j] = sensors.superficie[position];
          mapaCotas[sensors.posF - i][sensors.posC - j] = sensors.cota[position++];
        }
      break;
    case noroeast:
      mapaResultado[sensors.posF][sensors.posC - 1] = sensors.superficie[1];
      mapaCotas[sensors.posF][sensors.posC - 1] = sensors.cota[1];
      mapaResultado[sensors.posF - 1][sensors.posC - 1] = sensors.superficie[2];
      mapaCotas[sensors.posF - 1][sensors.posC - 1] = sensors.cota[2];
      mapaResultado[sensors.posF - 1][sensors.posC] = sensors.superficie[3];
      mapaCotas[sensors.posF - 1][sensors.posC] = sensors.cota[3];
      mapaResultado[sensors.posF][sensors.posC - 2] = sensors.superficie[4];
      mapaCotas[sensors.posF][sensors.posC - 2] = sensors.cota[4];
      mapaResultado[sensors.posF - 1][sensors.posC - 2] = sensors.superficie[5];
      mapaCotas[sensors.posF - 1][sensors.posC - 2] = sensors.cota[5];
      mapaResultado[sensors.posF - 2][sensors.posC - 2] = sensors.superficie[6];
      mapaCotas[sensors.posF - 2][sensors.posC - 2] = sensors.cota[6];
      mapaResultado[sensors.posF - 2][sensors.posC - 1] = sensors.superficie[7];
      mapaCotas[sensors.posF - 2][sensors.posC - 1] = sensors.cota[7];
      mapaResultado[sensors.posF - 2][sensors.posC] = sensors.superficie[8];
      mapaCotas[sensors.posF - 2][sensors.posC] = sensors.cota[8];
      mapaResultado[sensors.posF][sensors.posC - 3] = sensors.superficie[9];
      mapaCotas[sensors.posF][sensors.posC - 3] = sensors.cota[9];
      mapaResultado[sensors.posF - 1][sensors.posC - 3] = sensors.superficie[10];
      mapaCotas[sensors.posF - 1][sensors.posC - 3] = sensors.cota[10];
      mapaResultado[sensors.posF - 2][sensors.posC - 3] = sensors.superficie[11];
      mapaCotas[sensors.posF - 2][sensors.posC - 3] = sensors.cota[11];
      mapaResultado[sensors.posF - 3][sensors.posC - 3] = sensors.superficie[12];
      mapaCotas[sensors.posF - 3][sensors.posC - 3] = sensors.cota[12];
      mapaResultado[sensors.posF - 3][sensors.posC - 2] = sensors.superficie[13];
      mapaCotas[sensors.posF - 3][sensors.posC - 2] = sensors.cota[13];
      mapaResultado[sensors.posF - 3][sensors.posC - 1] = sensors.superficie[14];
      mapaCotas[sensors.posF - 3][sensors.posC - 1] = sensors.cota[14];
      mapaResultado[sensors.posF - 3][sensors.posC] = sensors.superficie[15];
      mapaCotas[sensors.posF - 3][sensors.posC] = sensors.cota[15];
      break;
  }
}



/**
 * @brief Determine whether a cell is traversable by the technician.
 * In this assignment, the technician can traverse the forest ('B') when wearing shoes.
 * @param f Cell row.
 * @param c Cell column.
 * @param hasShoes Indicates whether the agent has the shoes.
 * @return true if the cell is traversable.
 */
bool ComportamientoTecnico::isTraversableCellLevel0(int f, int c, bool hasShoes) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return isPath(mapaResultado[f][c]);  // Only 'C', 'S', 'D', and 'U' are traversable at level 0
}

/**
 * @brief Check whether the cell ahead is accessible based on elevation difference.
 * For the technician, the maximum elevation change is always 1.
 * @param current Current agent state (row, column, orientation).
 * @return true if the elevation difference with the cell ahead is acceptable.
 */
bool ComportamientoTecnico::isHeightAccessible(const ubicacion &current) {
  ubicacion aheadPosition = cellAhead(current);
  if (aheadPosition.f < 0 || aheadPosition.f >= mapaCotas.size() || aheadPosition.c < 0 || aheadPosition.c >= mapaCotas[0].size()) return false;
  int elevationDifference = abs(mapaCotas[aheadPosition.f][aheadPosition.c] - mapaCotas[current.f][current.c]);
  if (elevationDifference > 1) return false;
  return true;
}

/**
 * @brief Return the position (row, column) of the cell directly ahead of the agent.
 * Calculate the cell ahead based on the current orientation (8 directions).
 * @param current Current agent state (row, column, orientation).
 * @return State containing the row and column of the cell ahead.
 */
ubicacion ComportamientoTecnico::cellAhead(const ubicacion &current) const {
  ubicacion ahead = current;
  switch (current.brujula) {
    case 0: ahead.f--; break;                        // north
    case 1: ahead.f--; ahead.c++; break;     // northeast
    case 2: ahead.c++; break;                     // east
    case 3: ahead.f++; ahead.c++; break;     // southeast
    case 4: ahead.f++; break;                        // south
    case 5: ahead.f++; ahead.c--; break;     // southoeast
    case 6: ahead.c--; break;                     // oeast
    case 7: ahead.f--; ahead.c--; break;     // noroeast
  }
  return ahead;
}


/**
 * @brief Print an action plan's sequence to the console.
 *
 * @param plan  List of actions in the plan.
 */
void ComportamientoTecnico::printPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convert an action plan into a list of cells for
 *        display it on the 2D map.
 *
 * @param state    Starting state.
 * @param plan  List of actions in the plan.
 */
void ComportamientoTecnico::visualizePlan(const ubicacion &state,
                                            const list<Action> &plan)
{
   listaPlanCasillas.clear();
  ubicacion cst = state;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}
