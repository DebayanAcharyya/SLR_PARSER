#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <iomanip>
#include "lexer.h"
#include "token.h"

using namespace std;

// Class to represent an LR item
struct LR0Item {
    string lhs;       // Left-hand side of the production
    vector<string> rhs;       // Right-hand side of the production
    int dotPosition;  // Position of the dot in the right-hand side

    LR0Item(const string& l, const vector<string>& r, int pos) : lhs(l), rhs(r), dotPosition(pos) {}

    // Function to check if the item is complete (dot is at the end)
    bool isComplete() const {
        return dotPosition == rhs.size();
    }

    // Function to get the symbol after the dot (returns empty string if dot is at the end)
    string getNextSymbol() const {
        if (dotPosition < rhs.size())
            return rhs[dotPosition];
        else
            return "";
    }

    // Comparison operator for LR0Item
    bool operator<(const LR0Item& other) const {
        // First compare lhs
        if (lhs != other.lhs)
            return lhs < other.lhs;

        // Then compare rhs
        if (rhs.size() != other.rhs.size())
            return rhs.size() < other.rhs.size();
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (rhs[i] != other.rhs[i])
                return rhs[i] < other.rhs[i];
        }

        // Finally, compare dotPosition
        return dotPosition < other.dotPosition;
    }

    bool operator==(const LR0Item& other) const {
        return lhs == other.lhs && rhs == other.rhs && dotPosition == other.dotPosition;
    }
};

// Class to represent a production
class Production {
public:
    string left;            // Left-hand side of the production
    vector<string> right;   // Right-hand side of the production

    Production(const string& l, const vector<string>& r) : left(l), right(r) {}
};

// Define ActionType enum
enum class ActionType {
    SHIFT,
    REDUCE,
    GOTO,
    ACCEPT,
    ERROR
};

// Define Action struct
struct Action {
    ActionType type;
    int value; // For SHIFT and GOTO: state index; For REDUCE: production index
};

// Class to represent a grammar
class Grammar {
public:
    set<string> terminals;
    set<string> nonTerminals;
    string startSymbol;
    vector<Production> productions;
    vector<vector<LR0Item>> automaton; // Vector of LR(0) item sets
    map<pair<int, string>, int> transitions; // Map to store transitions
    map<string, set<string>> firstSets;
    map<string, set<string>> followSets;


    // Function to add transitions between LR(0) item sets
    void addTransition(int fromState, const string& symbol, int toState) {
        transitions[{fromState, symbol}] = toState;
    }

    // Add a production to the grammar
    void addProduction(const string& left, const vector<string>& right) {
        productions.emplace_back(left, right);
        nonTerminals.insert(left);
        for (const string& symbol : right) {
            if (isTerminal(symbol))
                terminals.insert(symbol);
        }
        
    }

    // Check if a symbol is a non-terminal
    bool isNonTerminal(const string& symbol) const {
        return nonTerminals.find(symbol) != nonTerminals.end();
    }

    // Check if a symbol is a terminal
    bool isTerminal(const string& symbol) const {
        return symbol != "#" && !isupper(symbol[0]); 
    }

    void computeFirstSets() {
        bool updated = true;
        while (updated) {
            updated = false;
            for (const Production& production : productions) {
                const string& left = production.left;
                const vector<string>& right = production.right;

                bool allDeriveEpsilon = true;
                for (const string& symbol : right) {
                    if(symbol == "#"){
                        allDeriveEpsilon = true;
                    }
                    else if (isTerminal(symbol)) {
                        updated |= firstSets[left].insert(symbol).second;
                        allDeriveEpsilon = false;
                        break;
                    } else {
                        for (const string& s : firstSets[symbol]) {
                            if (s != "#") {
                                updated |= firstSets[left].insert(s).second;
                            }
                        }
                        if (!firstSets[symbol].count("#")) {
                            allDeriveEpsilon = false;
                            break;
                        }
                    }
                }
                if (allDeriveEpsilon) {
                    updated |= firstSets[left].insert("#").second;
                }
            }
        }
    }


    

    // Function to compute follow sets
void computeFollowSets() {
    followSets[startSymbol].insert("$");
    bool updated = true;
    while (updated) {
        updated = false;
        for (const Production& production : productions) {
            const string& left = production.left;
            const vector<string>& right = production.right;

            for (size_t i = 0; i < right.size(); ++i) {
                const string& symbol = right[i];
                if (isNonTerminal(symbol)) {
                    bool allDeriveEpsilon = true;
                    for (size_t j = i + 1; j < right.size(); ++j) {
                        const string& nextSymbol = right[j];
                        if (isTerminal(nextSymbol)) {
                            updated |= followSets[symbol].insert(nextSymbol).second;
                            allDeriveEpsilon = false;
                            break;
                        } else if(isNonTerminal(nextSymbol)) {
                            for (const string& s : firstSets[nextSymbol]) {
                                if (s != "#") {
                                    updated |= followSets[symbol].insert(s).second;
                                }
                            }
                            if (!firstSets[nextSymbol].count("#")) {
                                allDeriveEpsilon = false;
                                break;
                            }
                        }
                    }
                    if (allDeriveEpsilon) {
                        for (const string& f : followSets[left]) {
                            updated |= followSets[symbol].insert(f).second;
                        }
                    }
                }
            }
        }
    }
}

    // Function to print the first sets
    void printFirstSets()  {
        cout << "FIRST Sets:" << endl;
        for (const auto& entry : firstSets) {
            cout << entry.first << ": { ";
            for (const string& symbol : entry.second) {
                cout << symbol << " ";
            }
            cout << "}" << endl;
        }
    }

    // Function to print the follow sets
    void printFollowSets() {
        cout << "Follow Sets:" << endl;
        for (const auto& pair : followSets) {
            cout << pair.first << ": [ ";
            for (const string& symbol : pair.second) {
                cout << symbol << " ";
            }
            cout << " ]" << endl;
        }
    }


    
    vector<LR0Item> generateInitialItemSet()  {
        vector<LR0Item> initialItemSet;
        // Add the initial LR(0) item
        
        for(auto &production: productions){
            if(production.left == startSymbol) {
                initialItemSet.emplace_back(production.left, production.right, 0);
                break;
            }
        }
        
        return calculateClosure(initialItemSet);
        
    }

    // Function to calculate closure of an LR(0) item set
    
vector<LR0Item> calculateClosure(vector<LR0Item>& itemSet)  {
    vector<LR0Item> closure = itemSet;

    bool addedNewItems = true;
    while (addedNewItems) {
        addedNewItems = false;
        // Iterate through each item in the item set
        for (size_t i = 0; i < closure.size(); ++i) {
            LR0Item& currentItem = closure[i];
            // Check if the next symbol after the dot is a non-terminal
            string nextSymbol = currentItem.getNextSymbol();
            if (!nextSymbol.empty() && isNonTerminal(nextSymbol)) {
                // Find productions with the next symbol as the left-hand side
                for (const Production& production : productions) {
                    if (production.left == nextSymbol) {
                        // Add new LR(0) item to the closure
                        LR0Item newItem(nextSymbol, production.right, 0);
                        // Check if the item is already in the closure
                       if (find(closure.begin(), closure.end(), newItem) == closure.end()) {
                            closure.push_back(newItem);
                            addedNewItems = true;
                        }
                    }
                }
            }
        }
    }
    return closure;
}

    // Function to compute Goto transitions
map<string, vector<LR0Item>> computeGoto(const vector<LR0Item>& itemSet, const string& symbol)  {
    map<string, vector<LR0Item>> gotoMap;
    // Iterate through each item in the item set
    for (const LR0Item& item : itemSet) {
        // Check if the next symbol after the dot matches the given symbol
        if (!item.isComplete() && item.getNextSymbol() == symbol) {
            // Create a new LR(0) item by shifting the dot
            LR0Item newItem(item.lhs, item.rhs, item.dotPosition + 1);
            // Add the new item to the corresponding Goto set
            gotoMap[symbol].push_back(newItem);
        }
    }
    return gotoMap;
}



// Function to construct the LR(0) automaton
void constructLR0Automaton() {
    
    vector<LR0Item> initialItemSet = generateInitialItemSet();
    automaton.push_back(initialItemSet);
    

    queue<int> itemSetQueue;
    itemSetQueue.push(0); // Push index of initial LR(0) item set

    // Repeat until no new LR(0) item sets are generated
    while (!itemSetQueue.empty()) {
        int currentIndex = itemSetQueue.front();
        itemSetQueue.pop();

        // Compute Goto transitions for each symbol
        for (const string& symbol : terminals) {
            map<string, vector<LR0Item>> gotoMap = computeGoto(automaton[currentIndex], symbol);
            if (!gotoMap.empty()) {
                vector<LR0Item> nextItemSet = calculateClosure(gotoMap[symbol]);
                int nextStateIndex = getOrCreateStateIndex(nextItemSet);
                addTransition(currentIndex, symbol, nextStateIndex);
                
                if (nextStateIndex == automaton.size()) {
                        automaton.push_back(nextItemSet);
                        itemSetQueue.push(nextStateIndex);
                    }
            }
        }
        for (const string& symbol : nonTerminals) {
            map<string, vector<LR0Item>> gotoMap = computeGoto(automaton[currentIndex], symbol);
            if (!gotoMap.empty()) {
                vector<LR0Item> nextItemSet = calculateClosure(gotoMap[symbol]);
                int nextStateIndex = getOrCreateStateIndex(nextItemSet);
                    addTransition(currentIndex, symbol, nextStateIndex);
                    if (nextStateIndex == automaton.size()) {
                        automaton.push_back(nextItemSet);
                        itemSetQueue.push(nextStateIndex);
                    }
            }
        }
    }

    
}
    // Helper function to get or create the index of an LR(0) item set in the automaton
    int getOrCreateStateIndex(const vector<LR0Item>& itemSet) {
        for (size_t i = 0; i < automaton.size(); ++i) {
            if (automaton[i] == itemSet) {
                return i;
            }
        }
        return automaton.size(); // Index of the new state
    }

    vector<vector<Action>> constructParsingTable(){
        cout << "No of terminals :" << terminals.size()<<endl;
        cout << "No of non Terminals : "<<nonTerminals.size()<<endl;
        vector<vector<Action>> parsingTable(automaton.size(), vector<Action>(terminals.size() + nonTerminals.size() + 1, {ActionType::ERROR, -1}));

        // Iterate over each state in the LR(0) automaton
        for (size_t stateIndex = 0; stateIndex < automaton.size(); ++stateIndex) {
            const auto& state =  automaton[stateIndex];

            // For each terminal, compute SHIFT action if applicable
            for (size_t i = 0; i < terminals.size(); ++i) {
                const string& terminal = *next(terminals.begin(), i);
                //auto gotoMap = computeGoto(state, terminal);
                auto transition = transitions.find({stateIndex,terminal});
                if(transition != transitions.end()){
                    int nextStateIndex = transition->second;
                    parsingTable[stateIndex][i] = {ActionType::SHIFT, nextStateIndex};
                }
                
            }

            // For each non-terminal, compute GOTO action if applicable
            for (size_t i = 0; i < nonTerminals.size(); ++i) {
                const string& nonTerminal = *next(nonTerminals.begin(), i);
                auto transition = transitions.find({stateIndex, nonTerminal});
                
                if(transition != transitions.end()){
                    int nextStateIndex = transition->second;
                    parsingTable[stateIndex][terminals.size() +1+ i] = {ActionType::GOTO, nextStateIndex};
                }
            }

            for (const LR0Item& item : state) {
                // If the item is a complete item, compute REDUCE action
                
                if (item.rhs[0] == "#" || item.isComplete()) {
                    int productionIndex = getProductionIndex(item.lhs, item.rhs);
                    if(productionIndex == -1) cout << "Prodution Not found"<<endl;
                    for (const string& terminal : followSets[item.lhs]) {
                        if (terminal == "$") {
                            // if( parsingTable[stateIndex][terminals.size()].type != ActionType::ERROR) cout << " Conflict found"<<endl;
                            if(item.lhs == startSymbol){
                                parsingTable[stateIndex][terminals.size()] = {ActionType::ACCEPT, -1};
                            }
                            else{
                                parsingTable[stateIndex][terminals.size()] = {ActionType::REDUCE, productionIndex };
                            }
                            
                        } else {
                            int terminalIndex = getTerminalIndex(terminal);
                            //if( parsingTable[stateIndex][terminalIndex].type != ActionType::ERROR) cout << " Conflict found"<<endl;
                            parsingTable[stateIndex][terminalIndex] = {ActionType::REDUCE, productionIndex};
                        }
                    }
                }
            }

        }
        return parsingTable;
    }

    void printParsingTable(vector<vector<Action>> &parsingTable){
        
        cout << "Parsing Table:" << endl;

        // Print column headers (terminals, non-terminals, and end of input)
       cout << setw(1) << " ";
        for (const string& terminal : terminals) {
            cout << setw(7) << terminal;
        }
        cout << setw(7) << "$";
        //cout << setw(7) << "| ";
        for (const string& nonTerminal : nonTerminals) {
            cout << setw(7) << nonTerminal;
        }
        cout << endl;
        
        // Print table contents
        for (size_t stateIndex = 0; stateIndex < parsingTable.size(); ++stateIndex) {
            cout << "I" << setw(5) << left << stateIndex << " ";
            for (size_t i = 0; i < parsingTable[stateIndex].size(); ++i) {
                const Action& action = parsingTable[stateIndex][i];

                if (action.type == ActionType::SHIFT) {
                    cout << setw(7) << left << ("S" + to_string(action.value));
                } else if (action.type == ActionType::GOTO) {
                    cout << setw(7) << left << ("G" + to_string(action.value));
                } else if (action.type == ActionType::REDUCE) {
                    cout << setw(7) << left << ("R" + to_string(action.value));
                } else if (action.type == ActionType::ERROR) {
                    cout << setw(7) << left << "X";
                } else if(action.type == ActionType::ACCEPT) {
                    cout << setw(7) << left << "A";
                } 
                
            }
            cout << endl;
        }
    }
    
    int getProductionIndex(const string& left, const vector<string>& right) const {
        for (size_t i = 0; i < productions.size(); ++i) {
            if (productions[i].left == left && productions[i].right == right) {
                return i;
            }
        }
        return -1; // Production not found
    }

    int getTerminalIndex(const string &terminal){
        auto it = find(terminals.begin(), terminals.end(), terminal);
        if (it != terminals.end()) {
            return distance(terminals.begin(), it);
        }
        return -1; // Terminal not found
    }

    int getNonTerminalIndex(const string &nonTerminal){
        auto it = find(nonTerminals.begin(), nonTerminals.end(), nonTerminal);
        if (it != nonTerminals.end()) {
            return terminals.size() + 1 + distance(nonTerminals.begin(), it);
        }
        return -1; // Non Terminal not found
    }


    // Function to parse the input string using the LR(0) parsing table
    bool parseInput(const vector<vector<Action>>& parsingTable, const vector<string>& inputTokens) {
    stack<int> stateStack;
    stateStack.push(0); // Push initial state onto stack
    int inputIndex = 0; // Index to track input tokens
    bool acceptReached = false;

    cout << "Parsing Steps:" << endl;
    cout << "---------------------------------------------" << endl;
    
    cout << "Stack\t\tInput\t\tAction" << endl;
    cout << "---------------------------------------------" << endl;

    while (!stateStack.empty()) {
        int currentState = stateStack.top();
        string currentInput = (inputIndex < inputTokens.size()) ? inputTokens[inputIndex] : "$";

        // Print stack and input
         
        cout  << currentState << "\t\t";
        for (size_t i = inputIndex; i < inputTokens.size(); ++i) {
            cout << inputTokens[i] << " ";
        }
        cout << "\t\t";

        // Check if ACCEPT state is reached
        if (parsingTable[currentState][terminals.size()].type == ActionType::ACCEPT) {
            cout << "ACCEPT" << endl;
            acceptReached = true;
            break;
        }
        

        // Check if the current input token is a terminal
        if (currentInput == "$" || terminals.find(currentInput) != terminals.end()) {
            int terminalIndex = currentInput == "$" ?   terminals.size() :  getTerminalIndex(currentInput);
            Action action = parsingTable[currentState][terminalIndex];
            if (action.type == ActionType::SHIFT) {
                cout << "SHIFT " << action.value << endl;
                stateStack.push(action.value);
                ++inputIndex;
            } else if (action.type == ActionType::REDUCE) {
                int productionIndex = action.value;
                vector<string>& productionRHS = productions[productionIndex].right; /* Get RHS of production */
                int numSymbolsToPop = productionRHS[0] == "#" ? 0 : productionRHS.size();
                for (int i = 0; i < numSymbolsToPop; ++i) {
                    stateStack.pop();
                }
                int newState = stateStack.top();
                const string& nonTerminal = productions[productionIndex].left; /*Get Non Terminal on LHS*/
                int nextState = parsingTable[newState][getNonTerminalIndex(nonTerminal)].value;/* Get next state from parsing table using nonTerminal */;
                stateStack.push(nextState);
                cout << "REDUCE by " << nonTerminal << " -> ";
                for (const string& symbol : productionRHS) {
                    cout << symbol << " ";
                }
                cout << endl;
            } else {
                cout << "ERROR: Invalid action" << endl;
                break;
            }
        } else {
            cout << "ERROR: Invalid input token" << endl;
            break;
        }
    }

    cout << "---------------------------------------------" << endl;

    return acceptReached;
}

    void ParseGrammar(string filename, string start){
        startSymbol = start;
        ifstream inputFile(filename);
        if (!inputFile.is_open()) {
            cerr << "Error: Unable to open file " << filename << endl;
            exit(EXIT_FAILURE);
        }
        string line;
        while (getline(inputFile, line)) {
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            string lhs;
            
            ss >> lhs; // Read LHS

            string arrow;
            ss >> arrow; // Read "->"

            string rhs;
            vector<string> rhsTokens;
            while (ss >> rhs) { // Read RHS tokens
                rhsTokens.push_back(rhs);
                
            }

            addProduction(lhs, rhsTokens);
        }

        inputFile.close();
    }

};




int main(int argc, char *argv[]) {

    
    Grammar grammar;
    string grammarfile = argv[1];
    string startSymbol = "M'";
    string filename = argv[2];
    grammar.ParseGrammar(grammarfile, startSymbol);
    

   grammar.constructLR0Automaton();
   vector<vector<LR0Item>> automaton = grammar.automaton;

    cout << "Productions:" << endl;
    for (const auto& production : grammar.productions) {
        cout << production.left << " -> ";
        for (const string& symbol : production.right) {
            cout << symbol << " ";
        }
        cout << endl;
    }
    cout <<  endl;

   /// Print terminals
    cout << "\nTerminals:" << endl;
    for (const auto& terminal : grammar.terminals) {
        cout << terminal << endl;
    }
    cout << endl;

    // Print non-terminals
    cout << "\nNon-terminals:" << endl;
    for (const auto& nonTerminal : grammar.nonTerminals) {
        cout << nonTerminal << endl;
    }

    // Display LR(0) item sets
    cout << "LR(0) Item Sets:" << endl;
    for (size_t i = 0; i < automaton.size(); ++i) {
        cout << "I" << i << ":" << endl;
        for (const LR0Item& item : automaton[i]) {
            cout << "  " << item.lhs << " -> ";
            for (size_t j = 0; j < item.rhs.size(); ++j) {
                if (static_cast<int>(j) == item.dotPosition)
                    cout << ". ";
                cout << item.rhs[j] << " ";
            }
            if (item.dotPosition == static_cast<int>(item.rhs.size()))
                cout << ". "; // Dot at the end
            cout << endl;
        }
        cout << endl;
    }
    cout << endl;

    // Display Transitions
    cout << "Transitions:" << endl;
    for (const auto& transition : grammar.transitions) {
        cout << "I" << transition.first.first << " --" << transition.first.second << "-> I" << transition.second << endl;
    }
    cout << endl;
    grammar.computeFirstSets();
    grammar.printFirstSets();
    cout << endl;
    grammar.computeFollowSets();
    grammar.printFollowSets();
    cout << endl;

    
    
    
    vector<string>tokens = getTokens(filename);
    tokens.push_back("$");
    for(auto token : tokens) cout << token <<endl;
    cout << endl;

    vector<vector<Action>> parsingTable = grammar.constructParsingTable();
    grammar.printParsingTable(parsingTable);
    cout << grammar.parseInput(parsingTable, tokens) << endl;

    cout << "Symbol Table : " << endl;
    for (const Token& token : SymbolTable) {
        std::cout << "Token: " << token.value << ", Line: " << token.line << ", Position: " << token.position << std::endl;
    }
    

    return 0;
}
