#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// STUDENT IMPLEMENTATION AREA
// =========================================================================

// ===============================================
// FOR LEVEL 0
// ===============================================

Action ComportamientoIngeniero::think(Sensores sensors)
{
  Action action = IDLE;
  currentInstant++;

  switch (sensors.nivel)
  {
  case 0:
    action = engineerBehaviorLevel0(sensors);
    break;
  case 1:
    action = engineerBehaviorLevel1(sensors);
    break;
  case 2:
    action = engineerBehaviorLevel2(sensors);
    break;
  case 3:
    action = engineerBehaviorLevel3(sensors);
    break;
  case 4:
    action = engineerBehaviorLevel4(sensors);
    break;
  case 5:
    action = engineerBehaviorLevel5(sensors);
    break;
  case 6:
    action = engineerBehaviorLevel6(sensors);
    break;
  }

  return action;
}

int ComportamientoIngeniero::exitLoop() {
  if (last_action == TURN_SL)
    return 1;
  else if (last_action == TURN_SR)
    return 3;
  else
    return 0;
}

/**
 * @brief Update the time map with the current time step at the given position.
 * @param f Row of the current cell.
 * @param c Column of the current cell.
 */
void ComportamientoIngeniero::updateTimeMap(int f, int c) {
  timeMap[f][c] = currentInstant;
}

char ComportamientoIngeniero::isViable(char collisionCell, char heightCell, int heightDifference, bool hasShoes){
  if (isCollisionViable(collisionCell))
    return isHeightViable(heightCell, heightDifference, hasShoes);
  else
    return 'P';
}

char ComportamientoIngeniero::isHeightViable(char cell, int difference, bool hasShoes){
  if (abs(difference) <= 1 || (hasShoes && abs(difference) <= 2))
    return cell;
  else
    return 'P';
}

bool ComportamientoIngeniero::isCollisionViable(char cell){
  if (cell != 't')
    return true;
  else
    return false;
}

/**
 * @brief Determine the best option among the three cells ahead.
 * @param i Terrain at surface position 1 (45 degrees left)
 * @param c Terrain at surface position 2 (directly ahead)
 * @param d Terrain at surface position 3 (45 degrees right)
 * @param hasShoes indicates whether the agent has the shoes
 * @return 2 for WALK, 1 for TURN_SL, and 3 for TURN_SR. 0 if nothing is interesting.
 */
int ComportamientoIngeniero::chooseInterestingCellLevel0(char i, char c, char d, bool hasShoes, const ubicacion &current){
  if (c == 'U') return 2;
  else if (i == 'U') return 1;
  else if (d == 'U') return 3;
  else if (!hasShoes){
    if (c == 'D') return 2;
    else if (i == 'D') return 1;
    else if (d == 'D') return 3;
  }

  if (c == 'D' || c == 'X')
    c = 'C';
  if (i == 'D' || i == 'X')
    i = 'C';
  if (d == 'D' || d == 'X')
    d = 'C';

  ubicacion frontPosition = cellAhead(current);
  ubicacion left45Position = cellAhead({current.f, current.c, (Orientacion)((current.brujula + 7) % 8)});
  ubicacion right45Position = cellAhead({current.f, current.c, (Orientacion)((current.brujula + 1) % 8)});

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

int ComportamientoIngeniero::chooseInterestingCellLevel1(char i, char c, char d, bool hasShoes, const ubicacion &current){
  if (!hasShoes){
    if (c == 'D') return 2;
    else if (i == 'D') return 1;
    else if (d == 'D') return 3;
  }

  if (c == 'D' || c == 'S' || c == 'U')
    c = 'C';
  if (i == 'D' || i == 'S' || i == 'U')
    i = 'C';
  if (d == 'D' || d == 'S' || d == 'U')
    d = 'C';

  ubicacion frontPosition = cellAhead(current);
  ubicacion left45Position = cellAhead({current.f, current.c, (Orientacion)((current.brujula + 7) % 8)});
  ubicacion right45Position = cellAhead({current.f, current.c, (Orientacion)((current.brujula + 1) % 8)});

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

// Initial levels (simple reactive behaviors)
Action ComportamientoIngeniero::engineerBehaviorLevel0(Sensores sensors)
{ 
  Action action = IDLE;
  ubicacion current = {sensors.posF, sensors.posC, sensors.rumbo};

  updateMap(sensors);
  updateTimeMap(sensors.posF, sensors.posC);

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

  char i = isViable(sensors.agentes[1], sensors.superficie[1], sensors.cota[1] - sensors.cota[0], hasShoes);
  char c = isViable(sensors.agentes[2], sensors.superficie[2], sensors.cota[2] - sensors.cota[0], hasShoes);
  char d = isViable(sensors.agentes[3], sensors.superficie[3], sensors.cota[3] - sensors.cota[0], hasShoes);

  int position = chooseInterestingCellLevel0(
    i,
    c,
    d,
    hasShoes,
    current
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
    case 4:
      action = JUMP;
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
bool ComportamientoIngeniero::isPath(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/**
 * @brief Reactive behavior of the engineer for level 1.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel1(Sensores sensors)
{
  Action action = IDLE;
  ubicacion current = {sensors.posF, sensors.posC, sensors.rumbo};

  // Follow a path until a waste treatment plant is found.
  // Write the vision sensor values to the maps.
  updateMap(sensors);
  updateTimeMap(sensors.posF, sensors.posC);

  // Update state variables
  if (sensors.superficie[0] == 'D') hasShoes = true;

  char i = isViable(sensors.agentes[1], sensors.superficie[1], sensors.cota[1] - sensors.cota[0], hasShoes);
  char c = isViable(sensors.agentes[2], sensors.superficie[2], sensors.cota[2] - sensors.cota[0], hasShoes);
  char d = isViable(sensors.agentes[3], sensors.superficie[3], sensors.cota[3] - sensors.cota[0], hasShoes);

  int position = chooseInterestingCellLevel1(
    i,
    c,
    d,
    hasShoes,
    current
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

// Advanced levels (search-based behaviors)
/**
 * @brief Behavior of the engineer for level 2 (search).
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel2(Sensores sensors)
{
  Action action = IDLE;
  if (!hasPlan){
  // Call the search method
    EstadoI start, fin;
    start.site.f = sensors.posF;
    start.site.c = sensors.posC;
    start.site.brujula = sensors.rumbo;
    start.shoes = hasShoes;
    fin.site.f = sensors.BelPosF;
    fin.site.c = sensors.BelPosC;
    actionPlan = breadthFirstSearchV2(start, fin, mapaResultado, mapaCotas);
    visualizePlan(start.site,actionPlan);
    hasPlan = actionPlan.size() != 0 ;
  }
  if (hasPlan and actionPlan.size()>0){
    action = actionPlan.front();
    actionPlan.pop_front();
  }
  if (actionPlan.size()== 0){
    hasPlan = false;
  }
  return action;
}

EstadoI nextTechnicianCell(const EstadoI &state);

bool isTechnicianCellAccessible(const EstadoI &state, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap){
  EstadoI next = nextTechnicianCell(state);
  
  if (next.site.f < 0 || next.site.f >= terrain.size()) return false;
    
  if (next.site.c < 0 || next.site.c >= terrain[0].size()) return false;
  
  bool check1 = terrain[next.site.f][next.site.c] != 'P' and terrain[next.site.f][next.site.c] != 'M';
  bool check2 = terrain[next.site.f][next.site.c] != 'B';
  
  bool check3;
  if (terrain[next.site.f][next.site.c] == '?'){
      check3 = true;
  } else if (state.shoes) {
      check3 = abs(heightMap[next.site.f][next.site.c] - heightMap[state.site.f][state.site.c]) <= 2;
  } else {
      check3 = abs(heightMap[next.site.f][next.site.c] - heightMap[state.site.f][state.site.c]) <= 1;
  }
  
  return check1 and check2 and check3;
}

bool ComportamientoIngeniero::isJumpViable(EstadoI next, const vector<vector<unsigned char>> &terrain){
  if (terrain[next.site.f][next.site.c] != 'P' && terrain[next.site.f][next.site.c] != 'M' && terrain[next.site.f][next.site.c] != 'B')
    return true;
  else
    return false;
}

bool ComportamientoIngeniero::isTechnicianJumpAccessible(const EstadoI &state, const vector<vector<unsigned char>> &terrain, const
  vector<vector<unsigned char>> &heightMap){
  EstadoI next = nextTechnicianCell(nextTechnicianCell(state));
  if (next.site.f < 0 || next.site.f >= terrain.size()) return false;
    
  if (next.site.c < 0 || next.site.c >= terrain[0].size()) return false;
  
  bool check1 = false, check2 = false, check3 = false, check4 = false;
  check1 = terrain[next.site.f][next.site.c] != 'P' and terrain[next.site.f][next.site.c] != 'M' and terrain[next.site.f][next.site.c] != 'B';
  check2 = terrain[next.site.f][next.site.c] != 'B';
  if (terrain[next.site.f][next.site.c] == '?'){
    check3 = true;
  } else if (state.shoes){
    check3 = abs(heightMap[next.site.f][next.site.c] - heightMap[state.site.f][state.site.c]) <= 2;
  }else{
    check3 = abs(heightMap[next.site.f][next.site.c] - heightMap[state.site.f][state.site.c]) <= 1;
  }
  check4 = heightMap[nextTechnicianCell(state).site.f][nextTechnicianCell(state).site.c] <= heightMap[state.site.f][state.site.c];
  return check1 and check2 and check3 and isJumpViable(nextTechnicianCell(state), terrain);
}

EstadoI nextTechnicianCell(const EstadoI &state){
  EstadoI next = state;
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

EstadoI ComportamientoIngeniero::applyEngineerAction(Action action, const EstadoI & state, const vector<vector<unsigned char>> &terrain, const vector<vector<unsigned char>> &heightMap){
  EstadoI next = state;
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
  case JUMP:
    if (isTechnicianJumpAccessible(state,terrain,heightMap)){
      next = nextTechnicianCell(nextTechnicianCell(state));
    }
    break;
  }

  if (terrain[next.site.f][next.site.c] == 'D') {
      next.shoes = true;
  }
  
  return next;
}

bool findNode (const NodoI & state, const list<NodoI> &nodeList){
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
list<Action> ComportamientoIngeniero::breadthFirstSearchV2(const EstadoI &start, const EstadoI &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap) {
  NodoI currentNode;
  list<NodoI> frontier;
  set<NodoI> explored;
  list<Action> path;

  currentNode.state = start;
  frontier.push_back(currentNode);

  while (!frontier.empty()) {
    currentNode = frontier.front();
    frontier.pop_front();

    if (explored.find(currentNode) != explored.end()) {
        continue;
    }

    if (currentNode.state.site.f == goal.site.f && currentNode.state.site.c == goal.site.c) {
        return currentNode.sequence;
    }

    explored.insert(currentNode);

    Action acciones[4] = {WALK, JUMP, TURN_SL, TURN_SR};

    for (int i = 0; i < 4; i++) {
        Action action = acciones[i];
        NodoI child = currentNode;
        child.state = applyEngineerAction(action, currentNode.state, terrain, heightMap);

        if (!(child.state == currentNode.state)) {
            child.sequence.push_back(action);

            if (child.state.site.f == goal.site.f && child.state.site.c == goal.site.c) {
                return child.sequence;
            }

              if (explored.find(child) == explored.end()) {
                  frontier.push_back(child);
              }
        }
    }
  }

  return path;
}

/**
 * @brief Behavior of the engineer for level 3.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel3(Sensores sensors)
{
  return IDLE;
}

/**
 * @brief Behavior of the engineer for level 4.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel4(Sensores sensors) {
  if (!hasPlan) {
    ubicacion belkanitaPosition;
    belkanitaPosition.f = sensors.BelPosF;
    belkanitaPosition.c = sensors.BelPosC;
    
    list<pair<int, int>> nearbyBases = findSortedBases(mapaResultado, belkanitaPosition);

    list<Paso> bestPlan; 
    list<Action> bestActions;
    int bestLimit = 999999;

    for (const auto& base : nearbyBases) {
      pair<int, int> goal = {base.first, base.second};
      
      int idealDistance = calculateHeuristic(belkanitaPosition.f, belkanitaPosition.c, goal.first, goal.second);
      
      if (idealDistance >= bestLimit) {
          continue;
      }

      pair<list<Paso>, list<Action>> currentPlan = calculatePipePlanActions(bestLimit, sensors.max_ecologico, goal, mapaResultado, mapaCotas, sensors);

      if (!currentPlan.first.empty()) {
        int currentInstallCount = currentPlan.first.size(); 
        
        if (bestPlan.empty() || currentInstallCount < bestLimit) {
            bestPlan = currentPlan.first; 
            bestActions = currentPlan.second;
            bestLimit = currentInstallCount; 
        }
      }
    }

    if (!bestPlan.empty()) {
        hasPlan = true; 
        pipePlan = bestPlan; 
        visualizePipeNetwork(bestPlan); 
    } else {
        hasPlan = true; 
    }
  }

  return IDLE;
}

list<pair<int, int>> ComportamientoIngeniero::findSortedBases(const vector<vector<unsigned char>> &map, const ubicacion &belkanita) {
  vector<pair<int, int>> bases;

  // 1. findNode all 'U' bases
  for (int f = 0; f < map.size(); f++) {
    for (int c = 0; c < map.size(); c++) {
      if (map[f][c] == 'U' && discardedPipes.find({f, c}) == discardedPipes.end()) {
        bases.push_back({f, c});
      }
    }
  }

  // 2. Sort by Euclidean distance from the Belkanita
  sort(bases.begin(), bases.end(), [&belkanita](const pair<int, int> &a, const pair<int, int> &b) {
    double distA = sqrt(pow(a.first - belkanita.f, 2) + pow(a.second - belkanita.c, 2));
    double distB = sqrt(pow(b.first - belkanita.f, 2) + pow(b.second - belkanita.c, 2));
    return distA < distB;
  });
  return list<pair<int, int>>(bases.begin(), bases.end());
}

int ComportamientoIngeniero::calculateHeuristic(int currentRow, int currentColumn, int goalRow, int goalColumn) {
    return abs(currentRow - goalRow) + abs(currentColumn - goalColumn);
}

pair<list<Paso>, list<Action>> ComportamientoIngeniero::calculatePipePlanActions(int minimumInstallCount, int maxEcologicalImpact, const pair<int, int> &destination, const vector<vector<unsigned char>> &terrainTypeMap, const vector<vector<unsigned char>> &terrainHeightMap, Sensores sensors) {

  priority_queue<Nodo_Nivel4, vector<Nodo_Nivel4>, greater<Nodo_Nivel4>> frontier;


  map<tuple<int, int, Orientacion, int, int, bool, bool>, vector<pair<int, int>>> explored;
  //tuple<int row, int column, Orientacion brujula, int effectiveHeight, int previousPipeHeight, bool pipeInstalled, bool terrainAltered>

  Nodo_Nivel4 initial;
  initial.state.site.f = sensors.BelPosF;
  initial.state.site.c = sensors.BelPosC;
  initial.state.site.brujula = sensors.rumbo;

  initial.currentHeight = terrainHeightMap[sensors.BelPosF][sensors.BelPosC];
  initial.previousPipeHeight = initial.currentHeight;
  initial.pipeInstalled = true;
  initial.terrainAltered = false;

  initial.gInstallCount = 0;
  initial.hDistance = calculateHeuristic(sensors.BelPosF, sensors.BelPosC, destination.first, destination.second);
  initial.totalFCost = initial.gInstallCount + initial.hDistance;

  initial.ecologicalImpact = 0;

  initial.stepSequence.push_back({sensors.BelPosF, sensors.BelPosC, 0});
  frontier.push(initial);

  while(!frontier.empty()) {
    Nodo_Nivel4 current = frontier.top();
    frontier.pop();

    if (current.gInstallCount >= minimumInstallCount) {
        continue; 
    }

    if (current.state.site.f == destination.first && current.state.site.c == destination.second && current.pipeInstalled) {
      return {current.stepSequence, current.actionSequence};
    }

    auto key = make_tuple(
        current.state.site.f, current.state.site.c, current.state.site.brujula,
        current.currentHeight, current.previousPipeHeight, current.pipeInstalled, current.terrainAltered
    );
    
    bool dominated = false;
    auto& paretoList = explored[key];
    
    for (const auto& entry : paretoList) {
        if (entry.first <= current.gInstallCount && entry.second <= current.ecologicalImpact) {
            dominated = true;
            break;
        }
    }
    
    if (dominated) {
        continue; 
    }
    
    paretoList.push_back({current.gInstallCount, current.ecologicalImpact});

    unsigned char currentTerrain = terrainTypeMap[current.state.site.f][current.state.site.c];

    bool atBelkanita = (current.gInstallCount == 0 && current.stepSequence.size() == 1);

    if((!current.pipeInstalled || atBelkanita) && !current.terrainAltered && currentTerrain != 'A') {
      
      //DIG
      if(current.currentHeight > 0) {
        int digEcologicalImpact = baseCost(DIG, currentTerrain);
        if (current.ecologicalImpact + digEcologicalImpact <= maxEcologicalImpact) {
          Nodo_Nivel4 hijoDig = current;
          hijoDig.currentHeight -= 1;
          
          // IMPORTANT: If we dig the outlet, the pipe's base elevation changes.
          if (atBelkanita) hijoDig.previousPipeHeight = hijoDig.currentHeight; 

          hijoDig.terrainAltered = true;
          hijoDig.ecologicalImpact += digEcologicalImpact;
          hijoDig.actionSequence.push_back(DIG);
          hijoDig.stepSequence.back().op = -1;
          frontier.push(hijoDig);
        }
      }
      
      //RAISE
      if(current.currentHeight < 9) {
        int raiseEcologicalImpact = baseCost(RAISE, currentTerrain);
        if (current.ecologicalImpact + raiseEcologicalImpact <= maxEcologicalImpact) {
          Nodo_Nivel4 hijoRaise = current;
          hijoRaise.currentHeight += 1;
          
          // IMPORTANT: If we raise the outlet, the pipe's base elevation changes.
          if (atBelkanita) hijoRaise.previousPipeHeight = hijoRaise.currentHeight;

          hijoRaise.terrainAltered = true;
          hijoRaise.ecologicalImpact += raiseEcologicalImpact;
          hijoRaise.actionSequence.push_back(RAISE);
          hijoRaise.stepSequence.back().op = 1;
          frontier.push(hijoRaise);
        }
      }
    }
    //INSTALL
    if (!current.pipeInstalled) {
      int difference = current.previousPipeHeight - current.currentHeight;
      if(difference == 0 || difference == 1) {
        
        int engineerImpact = baseCost(INSTALL, currentTerrain);
        int technicianImpact = 0;

        if (current.stepSequence.size() >= 2) {
            auto it = current.stepSequence.rbegin();
            it++;
            
            unsigned char previousTerrain = terrainTypeMap[it->fil][it->col];
            technicianImpact = baseCost(INSTALL, previousTerrain);
        } else {
            technicianImpact = engineerImpact; 
        }

        int installEcologicalImpact = engineerImpact + technicianImpact;

        if (current.ecologicalImpact + installEcologicalImpact <= maxEcologicalImpact) {
          Nodo_Nivel4 hijoInstall = current;
          hijoInstall.pipeInstalled = true;
          hijoInstall.previousPipeHeight = current.currentHeight;
          hijoInstall.gInstallCount += 1;
          
          hijoInstall.ecologicalImpact += installEcologicalImpact; 
          
          hijoInstall.totalFCost = hijoInstall.gInstallCount + hijoInstall.hDistance;
          hijoInstall.actionSequence.push_back(INSTALL);
          frontier.push(hijoInstall);
        }
      }
    }
    if (current.pipeInstalled) {
      for (Action action : {WALK, TURN_SL, TURN_SR}) {
        Nodo_Nivel4 hijoMovimiento = current;
        EstadoI estado_siguiente = applyEngineerAction(action, current.state, terrainTypeMap, terrainHeightMap);
        
        if (estado_siguiente != current.state) {
          if (action == WALK && current.state.site.brujula % 2 != 0) {
            continue; 
          }
          hijoMovimiento.state = estado_siguiente;

          if (action == WALK) { 
            hijoMovimiento.pipeInstalled = false; 
            hijoMovimiento.terrainAltered = false;
            
            hijoMovimiento.currentHeight = terrainHeightMap[hijoMovimiento.state.site.f][hijoMovimiento.state.site.c];
            hijoMovimiento.hDistance = calculateHeuristic(hijoMovimiento.state.site.f, hijoMovimiento.state.site.c, destination.first, destination.second);
            hijoMovimiento.totalFCost = hijoMovimiento.gInstallCount + hijoMovimiento.hDistance;

            hijoMovimiento.stepSequence.push_back({hijoMovimiento.state.site.f, hijoMovimiento.state.site.c, 0});
          }

          hijoMovimiento.actionSequence.push_back(action);
          frontier.push(hijoMovimiento);
        }
      }
    }
  }
  return {{}, {}};
}

int ComportamientoIngeniero::baseCost(Action action, unsigned char currentTerrain) {
    int cost = 0;

    if (action == WALK) {
        switch (currentTerrain) {
            case 'A': cost = 60; break;
            case 'H': cost = 6; break; 
            case 'S': cost = 3; break;
            default: cost = 1; break;
        }
    } else if (action == TURN_SL || action == TURN_SR) {
        switch (currentTerrain) {
            case 'A': cost = 5; break;
            case 'H': cost = 2; break;
            default: cost = 1; break;
        }
    } else if (action == DIG) {
        switch (currentTerrain) {
            case 'H': cost = 65; break;
            case 'S': cost = 40; break;
            case 'C': cost = 25; break;
            case 'U': cost = 25; break;
            default: cost = 50; break;
        }
    } else if (action == RAISE) {
        switch (currentTerrain) {
            case 'H': cost = 55; break;
            case 'S': cost = 30; break;
            case 'C': cost = 10; break;
            case 'U': cost = 10; break;
            default: cost = 40; break;
        }
    } else if (action == INSTALL) {
        switch (currentTerrain) {
            case 'A': cost = 50; break;
            case 'H': cost = 45; break;
            case 'S': cost = 25; break;
            case 'C': cost = 15; break;
            case 'U': cost = 15; break;
            default: cost = 30; break;
        }
    }
    return cost;
}

/**
 * @brief Behavior of the engineer for level 5.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel5(Sensores sensors) {
  Action action = IDLE;
  if (timeSinceWalk >= 0) timeSinceWalk++;

  // =======================================================
  // PHASE 1: GO TO THE BELKANITA (using level 2 logic)
  // =======================================================
  if (!atBelkanita) {
      if (!hasPlan) {
          EstadoI start, fin;
          start.site.f = sensors.posF;
          start.site.c = sensors.posC;
          start.site.brujula = sensors.rumbo;
          start.shoes = hasShoes;
          fin.site.f = sensors.BelPosF;
          fin.site.c = sensors.BelPosC;
          
          actionPlan = breadthFirstSearchV2(start, fin, mapaResultado, mapaCotas);
          visualizePlan(start.site, actionPlan);
          if (actionPlan.empty()) {
              atBelkanita = true;
              hasPlan = false;
          } else {
              hasPlan = true;
          }
      }

      if (hasPlan && !actionPlan.empty()) {
          action = actionPlan.front();
          actionPlan.pop_front();
          
          if (actionPlan.empty()) {
              atBelkanita = true; 
              hasPlan = false;
          }
          return action;
      }
  }

  // =======================================================
  // PHASE 2: PIPE PLANNING AND CONSTRUCTION
  // =======================================================
  if (atBelkanita) {
      if (!hasPlan) {
          ubicacion belkanitaPosition;
          belkanitaPosition.f = sensors.BelPosF;
          belkanitaPosition.c = sensors.BelPosC;
          
          list<pair<int, int>> nearbyBases = findSortedBases(mapaResultado, belkanitaPosition);

          list<Paso> bestPlan; 
          list<Action> bestActions;
          int bestLimit = 999999;

          for (const auto& base : nearbyBases) {
              pair<int, int> goal = {base.first, base.second};
              
              // Heuristic pruning
              int idealDistance = calculateHeuristic(belkanitaPosition.f, belkanitaPosition.c, goal.first, goal.second);
              if (idealDistance >= bestLimit) continue;

              pair<list<Paso>, list<Action>> currentPlan = calculatePipePlanActions(bestLimit, sensors.max_ecologico, goal, mapaResultado, mapaCotas, sensors);

              if (!currentPlan.first.empty()) {
                  int currentInstallCount = currentPlan.first.size(); 
                  if (bestPlan.empty() || currentInstallCount < bestLimit) {
                      bestPlan = currentPlan.first; 
                      bestActions = currentPlan.second;
                      bestLimit = currentInstallCount; 
                  }
              }
          }

          if (!bestPlan.empty()) {
              hasPlan = true;
              if (!bestActions.empty() && bestActions.front() == INSTALL) {
                  bestActions.pop_front();
              }
              actionPlan = bestActions;

              actionPlan.push_back(INSTALL);
              visualizePipeNetwork(bestPlan); 
          } else {
              hasPlan = true; // To avoid recalculating in an infinite loop
              return IDLE;
          }if (!bestPlan.empty()) {
              hasPlan = true;

              for (auto it = bestActions.begin(); it != bestActions.end(); ++it) {
                  if (*it == INSTALL) {
                      bestActions.erase(it);
                      break;
                  }
                  if (*it == WALK) {
                      // If we reach a WALK, it means we have already left the 
                      // starting cell. Stop searching.
                      break; 
                  }
              }
              // =======================================================

              actionPlan = bestActions;
              
              // Safety net: initialize walkOrientation with the heading 
              // currently used by the engineer, just in case.
              walkOrientation = sensors.rumbo;

              actionPlan.push_back(INSTALL);
              visualizePipeNetwork(bestPlan); 
          } else {
              hasPlan = true;
              constructionFailed = true;
              return IDLE;
          }
      }

        if (hasPlan && !actionPlan.empty()) {
            // --- 1. HEADING RECOVERY PHASE (Post-INSTALL) ---
            if (restoringOrientation) {
                if (sensors.rumbo == originalOrientation) {
                    restoringOrientation = false; 
                    // Do NOT call here anymore. It will be called automatically before the next WALK.
                } else {
                    int difference = (originalOrientation - sensors.rumbo + 8) % 8;
                    return (difference <= 4) ? TURN_SR : TURN_SL;
                }
            }
            Action nextAction = actionPlan.front();

            // --- 2. WALK INTERCEPTOR ---
            if (nextAction == WALK) {
                if (!calledBeforeWalk) {
                    calledBeforeWalk = true;
                    return COME; 
                } else {
                    calledBeforeWalk = false;
                    timeSinceWalk = 0;
                    // STORE THE DIRECTION OF TRAVEL
                    walkOrientation = sensors.rumbo; 
                    actionPlan.pop_front();
                    return WALK;
                }
            }

            if (nextAction == INSTALL) {
                if (!preparingInstall) {
                    preparingInstall = true;
                    originalOrientation = sensors.rumbo; 
                }

                int orientacion_esperada = (walkOrientation + 4) % 8;

                if (sensors.rumbo != orientacion_esperada) {
                    int difference = (orientacion_esperada - sensors.rumbo + 8) % 8;
                    return (difference <= 4) ? TURN_SR : TURN_SL;
                } 
                else {
                    if (sensors.enfrente) {
                        // Perfect synchronization achieved
                        preparingInstall = false;
                        restoringOrientation = true; 
                        actionPlan.pop_front();
                        return INSTALL;
                    } else {
                        return IDLE; 
                    }
                }
            }
            // --- 4. NORMAL ACTIONS (DIG, TURN...) ---
            else {
                actionPlan.pop_front();
                return nextAction;
            }
        }
  }
  if (atBelkanita && hasPlan && actionPlan.empty()) {
      // If we are here, construction is complete. 
      // Reset the engineer's state for next time.
      atBelkanita = false;
      hasPlan = false;
      preparingInstall = false;
      restoringOrientation = false;
      calledBeforeWalk = false;
      
      constructionFinished = true;
  }
  return IDLE;
}

/**
 * @brief Behavior of the engineer for level 6.
 * @param sensors Current sensor data.
 * @return Action to perform.
 */
Action ComportamientoIngeniero::engineerBehaviorLevel6(Sensores sensors) {
    updateMap(sensors);
    Action action = IDLE;

    EstadoI current;
    current.site.f = sensors.posF;
    current.site.c = sensors.posC;
    current.site.brujula = sensors.rumbo;
    current.shoes = hasShoes;

    switch (currentEngineerPhase) {
        
        case SEARCH_BELKANITA: {
            if (sensors.posF == sensors.BelPosF && sensors.posC == sensors.BelPosC) {
                currentEngineerPhase = EXPLORE_MAP; // We know its location; search for pipes.
            } else {
                EstadoI goal; goal.site.f = sensors.BelPosF; goal.site.c = sensors.BelPosC;
                action = reactiveAStarLevel6(current, goal, mapaResultado, mapaCotas);
            }
            break;
        }

        case EXPLORE_MAP: {
            pair<int, int> nueva_tuberia = findUnconnectedPipe(mapaResultado); 
            if (nueva_tuberia.first != -1) {
                targetPipeRow = nueva_tuberia.first;
                targetPipeColumn = nueva_tuberia.second;
                currentEngineerPhase = GO_TO_PIPE;
            } else {
                action = engineerBehaviorLevel1(sensors);
            }
            break;
        }

        case GO_TO_PIPE: {
            if (sensors.posF == targetPipeRow && sensors.posC == targetPipeColumn) {
                currentEngineerPhase = RETURN_TO_BELKANITA; // Pipe found; return while mapping the route.
            } else {
                EstadoI goal; goal.site.f = targetPipeRow; goal.site.c = targetPipeColumn;
                action = reactiveAStarLevel6(current, goal, mapaResultado, mapaCotas);
            }
            break;
        }

        case RETURN_TO_BELKANITA: {
            if (sensors.posF == sensors.BelPosF && sensors.posC == sensors.BelPosC) {
                // Route mapped! Hand control to level 5 so it can call the technician and build the pipe.
                currentEngineerPhase = RUN_LEVEL_5_ENGINEER; 
            } else {
                EstadoI goal; goal.site.f = sensors.BelPosF; goal.site.c = sensors.BelPosC;
                action = reactiveAStarLevel6(current, goal, mapaResultado, mapaCotas);
            }
            break;
        }

        case RUN_LEVEL_5_ENGINEER: {
            action = engineerBehaviorLevel5(sensors);

            if (constructionFailed){
              discardedPipes.insert({targetPipeRow, targetPipeColumn});
              constructionFailed = false;
              currentEngineerPhase = EXPLORE_MAP;
            }
            else if (constructionFinished) {
              discardedPipes.insert({targetPipeRow, targetPipeColumn});
              constructionFinished = false;
              currentEngineerPhase = EXPLORE_MAP;
            }
            break;
        }
    }
    return action;
}

pair<int, int> ComportamientoIngeniero::findUnconnectedPipe(const vector<vector<unsigned char>> &terrain){
  for (int f = 0; f < terrain.size(); f++){
    for (int c = 0; c < terrain[f].size(); c++){
      // We only care if it is 'U' AND is not in the discarded list
      if(terrain[f][c] == 'U' && discardedPipes.find({f, c}) == discardedPipes.end()){
        return {f, c};
      }
    }
  }
  return {-1, -1};
}


Action ComportamientoIngeniero::reactiveAStarLevel6(const EstadoI &start, const EstadoI &goal,
                                                const vector<vector<unsigned char>> &terrain,
                                                const vector<vector<unsigned char>> &heightMap) {

    priority_queue<NodoI_AEstrella, vector<NodoI_AEstrella>, greater<NodoI_AEstrella>> frontier;
    set<EstadoI> explored;

    NodoI_AEstrella startNode;
    startNode.state = start;
    startNode.g = 0;
    startNode.f = calculateHeuristic(start.site.f, start.site.c, goal.site.f, goal.site.c);
    
    frontier.push(startNode);

    while (!frontier.empty()) {
        NodoI_AEstrella currentNode = frontier.top();
        frontier.pop();

        // 3. Check whether we have reached the goal.
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

        Action acciones[4] = {WALK, JUMP, TURN_SL, TURN_SR};

        for (int i = 0; i < 4; i++) {
            Action action = acciones[i];
            NodoI_AEstrella child = currentNode;
            
            child.state = applyEngineerAction(action, currentNode.state, terrain, heightMap);

            if (!(child.state == currentNode.state)) {
                child.g = currentNode.g + 1; // Assume a cost of 1 per action
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

// =========================================================================
// PROVIDED FUNCTIONS
// =========================================================================

/**
 * @brief Update the terrain and elevation maps using sensor data.
 * @param sensors Current sensor data.
 */
void ComportamientoIngeniero::updateMap(Sensores sensors)
{
  mapaResultado[sensors.posF][sensors.posC] = sensors.superficie[0];
  mapaCotas[sensors.posF][sensors.posC] = sensors.cota[0];

  int position = 1;
  switch (sensors.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
 * @brief Determine whether a cell is traversable by the engineer.
 * @param f Cell row.
 * @param c Cell column.
 * @param hasShoes Indicates whether the agent has the shoes.
 * @return true if the cell is traversable (not a wall or cliff).
 */
bool ComportamientoIngeniero::isTraversableCellLevel0(int f, int c, bool hasShoes)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return isPath(mapaResultado[f][c]); // Only 'C', 'D', and 'U' are traversable at level 0
}

/**
 * @brief Check whether the cell ahead is accessible based on elevation difference.
 * For the engineer, the maximum elevation change is 1 without shoes and 2 with shoes.
 * @param current Current agent state (row, column, orientation, hasShoes).
 * @return true if the elevation difference with the cell ahead is acceptable.
 */
bool ComportamientoIngeniero::isHeightAccessible(const ubicacion &current, bool hasShoes)
{
  ubicacion aheadPosition = cellAhead(current);

  if (aheadPosition.f < 0 || aheadPosition.f >= mapaCotas.size() || aheadPosition.c < 0 || aheadPosition.c >= mapaCotas[0].size())
    return false;
  int elevationDifference = abs(mapaCotas[aheadPosition.f][aheadPosition.c] - mapaCotas[current.f][current.c]);
  if (hasShoes && elevationDifference > 2)
    return false;
  if (!hasShoes && elevationDifference > 1)
    return false;
  return true;
}

/**
 * @brief Return the position (row, column) of the cell directly ahead of the agent.
 * Calculate the cell ahead based on the current orientation (8 directions).
 * @param current Current agent state (row, column, orientation).
 * @return State containing the row and column of the cell ahead.
 */
ubicacion ComportamientoIngeniero::cellAhead(const ubicacion &current) const
{
  ubicacion ahead = current;
  switch (current.brujula)
  {
  case 0:
    ahead.f--;
    break; // north
  case 1:
    ahead.f--;
    ahead.c++;
    break; // northeast
  case 2:
    ahead.c++;
    break; // east
  case 3:
    ahead.f++;
    ahead.c++;
    break; // southeast
  case 4:
    ahead.f++;
    break; // south
  case 5:
    ahead.f++;
    ahead.c--;
    break; // southoeast
  case 6:
    ahead.c--;
    break; // oeast
  case 7:
    ahead.f--;
    ahead.c--;
    break; // noroeast
  }
  return ahead;
}

/**
 * @brief Print an action plan's sequence to the console.
 *
 * @param plan  List of actions in the plan.
 */
void ComportamientoIngeniero::printPlan(const list<Action> &plan)
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
 * @brief Print the coordinates and operations in a pipe plan.
 *
 * @param plan  List of steps (row, column, operation),
 *              where operation = -1 (DIG) or 1 (RAISE).
 */
void ComportamientoIngeniero::printPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::visualizePlan(const ubicacion &state,
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
        cst.f -= 2;
        break;
      case 1:
        cst.f -= 2;
        cst.c += 2;
        break;
      case 2:
        cst.c += 2;
        break;
      case 3:
        cst.f += 2;
        cst.c += 2;
        break;
      case 4:
        cst.f += 2;
        break;
      case 5:
        cst.f += 2;
        cst.c -= 2;
        break;
      case 6:
        cst.c -= 2;
        break;
      case 7:
        cst.f -= 2;
        cst.c -= 2;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
      break;
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

/**
 * @brief Convert a pipe plan into the list of cells used
 *        by the visualization system.
 *
 * @param state    Starting state (not used directly).
 * @param plan  List of steps in the pipe plan.
 */
void ComportamientoIngeniero::visualizePipeNetwork(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
