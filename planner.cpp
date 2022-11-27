#include <iostream>
#include <fstream>
// #include <boost/functional/hash.hpp>
#include <regex>
#include <unordered_set>
#include <set>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <queue>
#include <chrono>
#include "Helper_funcs.h"
#define SYMBOLS 0
#define INITIAL 1
#define GOAL 2
#define ACTIONS 3
#define ACTION_DEFINITION 4
#define ACTION_PRECONDITION 5
#define ACTION_EFFECT 6
#define DEBUG true 

class GroundedCondition;
class Condition;
class GroundedAction;
class Action;
class Env;

using namespace std;

bool print_status = true;

class GroundedCondition
{
private:
    string predicate;
    list<string> arg_values;
    bool truth = true;

public:
    GroundedCondition(string predicate, list<string> arg_values, bool truth = true)
    {
        this->predicate = predicate;
        this->truth = truth;  // fixed
        for (string l : arg_values)
        {
            this->arg_values.push_back(l);
        }
    }

    GroundedCondition(const GroundedCondition& gc)
    {
        this->predicate = gc.predicate;
        this->truth = gc.truth;  // fixed
        for (string l : gc.arg_values)
        {
            this->arg_values.push_back(l);
        }
    }

    string get_predicate() const
    {
        return this->predicate;
    }
    list<string> get_arg_values() const
    {
        return this->arg_values;
    }

    bool get_truth() const
    {
        return this->truth;
    }

    friend ostream& operator<<(ostream& os, const GroundedCondition& pred)
    {
        os << pred.toString() << " ";
        return os;
    }

    bool operator==(const GroundedCondition& rhs) const
    {
        if (this->predicate != rhs.predicate || this->arg_values.size() != rhs.arg_values.size())
            return false;

        auto lhs_it = this->arg_values.begin();
        auto rhs_it = rhs.arg_values.begin();

        while (lhs_it != this->arg_values.end() && rhs_it != rhs.arg_values.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }

        if (this->truth != rhs.get_truth()) // fixed
            return false;

        return true;
    }

    string toString() const
    {
        string temp = "";
        temp += this->predicate;
        temp += "(";
        for (string l : this->arg_values)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct GroundedConditionComparator
{
    bool operator()(const GroundedCondition& lhs, const GroundedCondition& rhs) const
    {
        return lhs == rhs;
    }
};

struct GroundedConditionHasher
{
    size_t operator()(const GroundedCondition& gcond) const
    {
        return hash<string>{}(gcond.toString());
    }
};

class Condition
{
private:
    string predicate;
    list<string> args;
    bool truth;

public:
    Condition(string pred, list<string> args, bool truth)
    {
        this->predicate = pred;
        this->truth = truth;
        for (string ar : args)
        {
            this->args.push_back(ar);
        }
    }

    string get_predicate() const
    {
        return this->predicate;
    }

    list<string> get_args() const
    {
        return this->args;
    }

    bool get_truth() const
    {
        return this->truth;
    }

    friend ostream& operator<<(ostream& os, const Condition& cond)
    {
        os << cond.toString() << " ";
        return os;
    }

    bool operator==(const Condition& rhs) const // fixed
    {

        if (this->predicate != rhs.predicate || this->args.size() != rhs.args.size())
            return false;

        auto lhs_it = this->args.begin();
        auto rhs_it = rhs.args.begin();

        while (lhs_it != this->args.end() && rhs_it != rhs.args.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }

        if (this->truth != rhs.get_truth())
            return false;

        return true;
    }

    string toString() const
    {
        string temp = "";
        if (!this->truth)
            temp += "!";
        temp += this->predicate;
        temp += "(";
        for (string l : this->args)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct ConditionComparator
{
    bool operator()(const Condition& lhs, const Condition& rhs) const
    {
        return lhs == rhs;
    }
};

struct ConditionHasher
{
    size_t operator()(const Condition& cond) const
    {
        return hash<string>{}(cond.toString());
    }
};

class Action
{
private:
    string name;
    list<string> args;
    unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    unordered_set<Condition, ConditionHasher, ConditionComparator> effects;

public:
    Action(string name, list<string> args,
        unordered_set<Condition, ConditionHasher, ConditionComparator>& preconditions,
        unordered_set<Condition, ConditionHasher, ConditionComparator>& effects)
    {
        this->name = name;
        for (string l : args)
        {
            this->args.push_back(l);
        }
        for (Condition pc : preconditions)
        {
            this->preconditions.insert(pc);
        }
        for (Condition pc : effects)
        {
            this->effects.insert(pc);
        }
    }
    string get_name() const
    {
        return this->name;
    }
    list<string> get_args() const
    {
        return this->args;
    }
    unordered_set<Condition, ConditionHasher, ConditionComparator> get_preconditions() const
    {
        return this->preconditions;
    }
    unordered_set<Condition, ConditionHasher, ConditionComparator> get_effects() const
    {
        return this->effects;
    }

    bool operator==(const Action& rhs) const
    {
        if (this->get_name() != rhs.get_name() || this->get_args().size() != rhs.get_args().size())
            return false;

        return true;
    }

    friend ostream& operator<<(ostream& os, const Action& ac)
    {
        os << ac.toString() << endl;
        os << "Precondition: ";
        for (Condition precond : ac.get_preconditions())
            os << precond;
        os << endl;
        os << "Effect: ";
        for (Condition effect : ac.get_effects())
            os << effect;
        os << endl;
        return os;
    }

    string toString() const
    {
        string temp = "";
        temp += this->get_name();
        temp += "(";
        for (string l : this->get_args())
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

struct ActionComparator
{
    bool operator()(const Action& lhs, const Action& rhs) const
    {
        return lhs == rhs;
    }
};

struct ActionHasher
{
    size_t operator()(const Action& ac) const
    {
        return hash<string>{}(ac.get_name());
    }
};

class Env
{
private:
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> initial_conditions;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> goal_conditions;
    unordered_set<Action, ActionHasher, ActionComparator> actions;
    unordered_set<string> symbols;
    vector<string> symbols_vec;

public:
    void remove_initial_condition(GroundedCondition gc)
    {
        this->initial_conditions.erase(gc);
    }
    void add_initial_condition(GroundedCondition gc)
    {
        this->initial_conditions.insert(gc);
    }
    void add_goal_condition(GroundedCondition gc)
    {
        this->goal_conditions.insert(gc);
    }
    void remove_goal_condition(GroundedCondition gc)
    {
        this->goal_conditions.erase(gc);
    }
    void add_symbol(string symbol)
    {
        symbols.insert(symbol);
    }
    //Added by AF below
    void add_symbol_vec(string symbol)
    {
        symbols_vec.push_back(symbol);
    }
    //Added by AF above
    void add_symbols(list<string> symbols)
    {
        for (string l : symbols)
            this->symbols.insert(l);
    }
    //Added by AF below
    void add_symbols_vec(list<string> symbol)
    {
        for (string l : symbols)
            this->symbols_vec.push_back(l);
    }
    //Added by AF above
    void add_action(Action action)
    {
        this->actions.insert(action);
    }

    Action get_action(string name)
    {
        for (Action a : this->actions)
        {
            if (a.get_name() == name)
                return a;
        }
        throw runtime_error("Action " + name + " not found!");
    }
    unordered_set<string> get_symbols() const
    {
        return this->symbols;
    }
    // Functions added by AF below
    vector<string> get_symbols_vec()
    {
        return this->symbols_vec;
    }
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_initial_condition () const
    {
        return this->initial_conditions;
    }
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> get_goal_condition () const
    {
        return this->goal_conditions;
    }
    list<Action> get_actions()
    {
        list<Action> available_actions;
        for(Action a:actions)
        {
            available_actions.push_back(a);
        }
        return available_actions;
    }
    //Functions added by AF above

    friend ostream& operator<<(ostream& os, const Env& w)
    {
        os << "***** Environment *****" << endl << endl;
        os << "Symbols: ";
        for (string s : w.get_symbols())
            os << s + ",";
        os << endl;
        os << "Initial conditions: ";
        for (GroundedCondition s : w.initial_conditions)
            os << s;
        os << endl;
        os << "Goal conditions: ";
        for (GroundedCondition g : w.goal_conditions)
            os << g;
        os << endl;
        os << "Actions:" << endl;
        for (Action g : w.actions)
            os << g << endl;
        cout << "***** Environment Created! *****" << endl;
        return os;
    }
};

class GroundedAction
{
private:
    string name;
    list<string> arg_values;

public:
    // Code Added by AF below
    GroundedAction(string name,vector<string> arg_values)
    :name{name}
    {
        this->name = name;
        for (string ar:arg_values)
        {
            this->arg_values.push_back(ar);
        }
    }
    GroundedAction(string name, list<string> arg_values)
    :name{name}
    {
        this->name = name;
        for (string ar : arg_values)
        {
            this->arg_values.push_back(ar);
        }
    }
    //Code Added by AF Above

    string get_name() const
    {
        return this->name;
    }

    list<string> get_arg_values() const
    {
        return this->arg_values;
    }

    bool operator==(const GroundedAction& rhs) const
    {
        if (this->name != rhs.name || this->arg_values.size() != rhs.arg_values.size())
            return false;

        auto lhs_it = this->arg_values.begin();
        auto rhs_it = rhs.arg_values.begin();

        while (lhs_it != this->arg_values.end() && rhs_it != rhs.arg_values.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }
        return true;
    }

    friend ostream& operator<<(ostream& os, const GroundedAction& gac)
    {
        os << gac.toString() << " ";
        return os;
    }

    string toString() const
    {
        string temp = "";
        temp += this->name;
        temp += "(";
        for (string l : this->arg_values)
        {
            temp += l + ",";
        }
        temp = temp.substr(0, temp.length() - 1);
        temp += ")";
        return temp;
    }
};

list<string> parse_symbols(string symbols_str)
{
    list<string> symbols;
    size_t pos = 0;
    string delimiter = ",";
    while ((pos = symbols_str.find(delimiter)) != string::npos)
    {
        string symbol = symbols_str.substr(0, pos);
        symbols_str.erase(0, pos + delimiter.length());
        symbols.push_back(symbol);
    }
    symbols.push_back(symbols_str);
    return symbols;
}

//------------------Code added by AF below
string unordered_set_to_string(unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator>& conditions_set)
{
    string cond_string ="";
    for(auto it = conditions_set.begin(); it!=conditions_set.end();++it)
    {
        GroundedCondition cond = *it;
        cond_string += cond.toString();
    }
    return cond_string;
}
class node 
{
    private:
        unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> node_conditions; //Set of conditions in unordereds set form 
        string string_conditions; //A string containing the same set of conditions
        double g,h,f; //Cost values for A*
        node* parent;// Node pointer for backtraking 
        GroundedAction applied_action;//We want to save the action that was applied to get to this node 
    public:
        
        node(unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> state, double g, double h,node* parent,GroundedAction action)
        :node_conditions{state},string_conditions{unordered_set_to_string(state)},g{g},h{h},parent{parent},applied_action{action}
        {
            this->f= g+h;
        }

        node(unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> state,GroundedAction null_action)
        :node_conditions{state},string_conditions{unordered_set_to_string(state)},applied_action{null_action}{
            this-> g =numeric_limits<double>::infinity();
            this-> h = 0; 
            this->f=g+h;
        }
        void set_g(double g)
        {
            this->g = g;
        }
        void set_h(double h)
        {
            this->h = h;
        }
        void set_f()
        {
            this->f = g+h;
        }
        void set_parent(node* parent)
        {
            this->parent = parent;
        }
        void set_applied_action(GroundedAction action)
        {
            this->applied_action = action;
        }
        double get_f_value() const
        {
            return this->f;
        }
        double get_g_value() const
        {
            return this->g; 
        }
        node* get_parent() const
        {
            return this->parent;
        }
        GroundedAction get_applied_action()
        {
            return this->applied_action;
        }
        string get_condition() const
        {
            return this->string_conditions;
        }
        auto get_unord_set_cond() const
        {
            return this->node_conditions;
        }

};

struct custom
{
	bool operator() (node* a,node* b) const{
    return a->get_f_value() > b->get_f_value();};
};

struct nodeComparator
{
    bool operator()(const node* lhs,const node* rhs) const
    {   
        auto lhs_cond = lhs->get_condition();
        auto rhs_cond = rhs->get_condition();

        return lhs_cond == rhs_cond;
    }
};

struct nodeHasher
{
    size_t operator()(const node* node_cond) const
    {
        auto condition_strong = node_cond->get_condition();
        return hash<string>{}(condition_strong);
    }
};
//------------------Code added by AF above

Env* create_env(char* filename)
{
    ifstream input_file(filename);
    Env* env = new Env();
    regex symbolStateRegex("symbols:", regex::icase);
    regex symbolRegex("([a-zA-Z0-9_, ]+) *");
    regex initialConditionRegex("initialconditions:(.*)", regex::icase);
    regex conditionRegex("(!?[A-Z][a-zA-Z_]*) *\\( *([a-zA-Z0-9_, ]+) *\\)");
    regex goalConditionRegex("goalconditions:(.*)", regex::icase);
    regex actionRegex("actions:", regex::icase);
    regex precondRegex("preconditions:(.*)", regex::icase);
    regex effectRegex("effects:(.*)", regex::icase);
    int parser = SYMBOLS;

    unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    unordered_set<Condition, ConditionHasher, ConditionComparator> effects;
    string action_name;
    string action_args;

    string line;
    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            string::iterator end_pos = remove(line.begin(), line.end(), ' ');
            line.erase(end_pos, line.end());

            if (line == "")
                continue;

            if (parser == SYMBOLS)
            {
                smatch results;
                if (regex_search(line, results, symbolStateRegex))
                {
                    line = line.substr(8);
                    sregex_token_iterator iter(line.begin(), line.end(), symbolRegex, 0);
                    sregex_token_iterator end;

                    env->add_symbols(parse_symbols(iter->str()));  // fixed

                    env->add_symbols_vec(parse_symbols(iter->str()));

                    parser = INITIAL;
                }
                else
                {
                    cout << "Symbols are not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == INITIAL)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, initialConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
                        iter++;

                        if (predicate[0] == '!')
                        {
                            env->remove_initial_condition(
                                GroundedCondition(predicate.substr(1), parse_symbols(args)));
                        }
                        else
                        {
                            env->add_initial_condition(
                                GroundedCondition(predicate, parse_symbols(args)));
                        }
                    }

                    parser = GOAL;
                }
                else
                {
                    cout << "Initial conditions not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == GOAL)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, goalConditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
                        iter++;

                        if (predicate[0] == '!')
                        {
                            env->remove_goal_condition(
                                GroundedCondition(predicate.substr(1), parse_symbols(args)));
                        }
                        else
                        {
                            env->add_goal_condition(
                                GroundedCondition(predicate, parse_symbols(args)));
                        }
                    }

                    parser = ACTIONS;
                }
                else
                {
                    cout << "Goal conditions not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTIONS)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, actionRegex))
                {
                    parser = ACTION_DEFINITION;
                }
                else
                {
                    cout << "Actions not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_DEFINITION)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, conditionRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;
                    // name
                    action_name = iter->str();
                    iter++;
                    // args
                    action_args = iter->str();
                    iter++;

                    parser = ACTION_PRECONDITION;
                }
                else
                {
                    cout << "Action not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_PRECONDITION)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, precondRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
                        iter++;

                        bool truth;

                        if (predicate[0] == '!')
                        {
                            predicate = predicate.substr(1);
                            truth = false;
                        }
                        else
                        {
                            truth = true;
                        }

                        Condition precond(predicate, parse_symbols(args), truth);
                        preconditions.insert(precond);
                    }

                    parser = ACTION_EFFECT;
                }
                else
                {
                    cout << "Precondition not specified correctly." << endl;
                    throw;
                }
            }
            else if (parser == ACTION_EFFECT)
            {
                const char* line_c = line.c_str();
                if (regex_match(line_c, effectRegex))
                {
                    const std::vector<int> submatches = { 1, 2 };
                    sregex_token_iterator iter(
                        line.begin(), line.end(), conditionRegex, submatches);
                    sregex_token_iterator end;

                    while (iter != end)
                    {
                        // name
                        string predicate = iter->str();
                        iter++;
                        // args
                        string args = iter->str();
                        iter++;

                        bool truth;

                        if (predicate[0] == '!')
                        {
                            predicate = predicate.substr(1);
                            truth = false;
                        }
                        else
                        {
                            truth = true;
                        }

                        Condition effect(predicate, parse_symbols(args), truth);
                        effects.insert(effect);
                    }

                    env->add_action(
                        Action(action_name, parse_symbols(action_args), preconditions, effects));

                    preconditions.clear();
                    effects.clear();
                    parser = ACTION_DEFINITION;
                }
                else
                {
                    cout << "Effects not specified correctly." << endl;
                    throw;
                }
            }
        }
        input_file.close();
    }

    else
        cout << "Unable to open file";

    return env;
}

//------------------Code added by AF below
list<GroundedAction> gen_fsble_actions(
    Env* env,
    vector<vector<vector<string>>>& all_comb,
    Action& action,
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator>& actual_condition
) //This function returns a list of actions that obey the preconditions
{
    unordered_set<Condition, ConditionHasher, ConditionComparator> preconditions;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> succ_precondition;
    auto a_args = action.get_args(); //Action arguments
    auto a_predicate = action.get_name();//Action predicates
    list<GroundedAction> feasible_actions; // List containing actions
    vector<string> combs_arg; 
    int arg_idx = 0; 
    int comb_i = 0;
    int  a_arg_size = action.get_args().size();
    bool equal_cond = false;
    unordered_map<string,int> args_2_idx; 
    //Creating a structure to map the args in actions to idx 
    for (auto it=a_args.begin(); it != a_args.end(); ++it)
    {
        args_2_idx[*it] = arg_idx;
        ++arg_idx;
    }
    preconditions = action.get_preconditions();
    //While loop to create all possible combinations of actions
    while(comb_i < all_comb[a_arg_size-1].size())
    {
        //Creating a vector with all possible combinations of the same size of the arguments in the action(Vector of vectors of strings)
        combs_arg = all_comb[a_arg_size-1][comb_i];
        //After creating the map, now is time to check preconditions
        for(Condition c:preconditions)
            {
                auto c_predicate = c.get_predicate();
                auto c_args = c.get_args();
                list<string> precond_arg; 
                for (auto it=c_args.begin(); it != c_args.end(); ++it)
                {   
                    if(args_2_idx.find(*it)!= args_2_idx.end())
                    {
                        int symbol_idx = args_2_idx[*it];
                        string symbol_precond = combs_arg[symbol_idx];
                        precond_arg.push_back(symbol_precond);
                    }
                    else
                    {
                        precond_arg.push_back(*it);
                    }
                }
                auto artificial_precond = GroundedCondition(c_predicate,precond_arg);
                succ_precondition.insert(artificial_precond);
            }
        for(GroundedCondition cond:succ_precondition)
        {
            if(actual_condition.find(cond) != actual_condition.end())
                {
                    equal_cond = true;
                    continue;
                }
            else
                {
                    equal_cond = false;
                    succ_precondition.clear();
                    break;
                }
        }
        ++comb_i;
        if(equal_cond)
        {
            GroundedAction fsble_grnd_action = GroundedAction(a_predicate,combs_arg);
            feasible_actions.push_back(fsble_grnd_action);
        }
    }
    return feasible_actions;
    
}

double getMatchedConditions(node* actual_node,unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator>& goal_condition) //Returns the number of conditions that matched with a "goal" condition. 
{
    int matched_conditions = 0; 
    auto actual_gc = actual_node->get_unord_set_cond(); 
    for(GroundedCondition c:actual_gc)
        {
            if(goal_condition.find(c)!= goal_condition.end())
            {
                ++matched_conditions;
            }
        }
    return matched_conditions;
}
unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> execute_action(
    Env* env,
    Action& action,
    GroundedAction& feasible_action,
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator>& actual_condition
)//This executes the effects of the action and remove any condition that is false 
{
    unordered_set<Condition, ConditionHasher, ConditionComparator> a_effects;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> succ_effect;
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> affected_condition;
    unordered_map<string,int> args_2_idx; 

    affected_condition = actual_condition;
    a_effects = action.get_effects();
    auto a_args = action.get_args();
    auto a_predicate = action.get_name();
    auto fsbl_action_args_lst = feasible_action.get_arg_values();
    vector<string> feasible_action_args;
    int arg_idx = 0; 
    int comb_i = 0;
    int  a_arg_size = action.get_args().size();
    bool applied_effect = false;
    //Creating a structure to map the args in actions to idx 
    for (auto it=a_args.begin(); it != a_args.end(); ++it)
    {
        args_2_idx[*it] = arg_idx;
        ++arg_idx;
    }
    for (auto it = fsbl_action_args_lst.begin(); it != fsbl_action_args_lst.end(); ++it)
    {
        feasible_action_args.push_back(*it);
    }
    //While loop to create all possible combinations of actions
    //Creating a vector with all possible combinations of the same size of the arguments in the action(Vector of vectors of strings)
    //After creating the map, now is time to check effects
    for(Condition c:a_effects)
        {
            auto c_predicate = c.get_predicate();
            auto c_args = c.get_args();
            list<string> precond_arg; 
            for (auto it=c_args.begin(); it != c_args.end(); ++it)
            {   
                if(args_2_idx.find(*it)!= args_2_idx.end())
                {
                    int symbol_idx = args_2_idx[*it];
                    string symbol_precond = feasible_action_args[symbol_idx];
                    precond_arg.push_back(symbol_precond);
                }
                else
                {
                    precond_arg.push_back(*it);
                }
            }
            auto artificial_precond = GroundedCondition(c_predicate,precond_arg,c.get_truth());
            succ_effect.insert(artificial_precond); 
        }
    for(auto itr = succ_effect.begin(); itr != succ_effect.end();++itr) //FOr every grounded "effect" remove any state that is fasle and insert any new ones
    {
        GroundedCondition gr_effect = *itr;
        if(!gr_effect.get_truth())
            {
                affected_condition.erase(GroundedCondition(gr_effect.get_predicate(),gr_effect.get_arg_values()));
            }
        else
            {
                affected_condition.insert(GroundedCondition(gr_effect.get_predicate(),gr_effect.get_arg_values()));
            }
    }

    return affected_condition;
}

list<GroundedAction> ComputeSymbolicAstar(
    Env* env,
    results& res
)
{    
    priority_queue<node*,vector<node*>,custom> open_ptr;
    unordered_set<node*,nodeHasher,nodeComparator> close_list;
    map<string,double> open_ptr_track; //Keeps track of the g-value of every node in open list 
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> successor_condition;//A set of conditions for the "successor" node
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> initial_condition = env-> get_initial_condition();//Initial conditions
    unordered_set<GroundedCondition, GroundedConditionHasher, GroundedConditionComparator> goal_condition = env-> get_goal_condition();//Goal conditions
    list<node*> path; 
    list<GroundedAction> path_action;
    list<Action> available_act = env->get_actions(); //ALl the available actions of the environment 
    vector<vector<vector<string>>> all_comb;
    int goal_num_gc = goal_condition.size();
    int max_size = env->get_symbols().size();
    //Creating all possible combinations of symbols for all possible subsets. 
    for(int i = 1; i <= max_size;++i)
    {
        vector<vector<string>> combs = getAllCombinations(env->get_symbols_vec(),i);
        all_comb.push_back(combs);
    }
    
    //Initializing Open list with start node
    node* start_node = new node(initial_condition,0,0,nullptr,GroundedAction("No_action",list<string> {"A"}));
    // double start_h_val = goal_num_gc - getMatchedConditions(start_node,goal_condition);
    // start_node->set_h(start_h_val);
    // start_node->get_f_value();
    open_ptr.emplace(start_node);

    while(!open_ptr.empty())
    {
        node* actual_node = open_ptr.top();  //Get node with min f-value in open list 
        auto actual_gc = actual_node->get_unord_set_cond(); 
        close_list.insert(actual_node); // insert node into close list
        open_ptr.pop(); //remove node from open list
        //Goal Checking 
        int match_cond = getMatchedConditions(actual_node,goal_condition); //returns how many conditions of the goal are met
        if(match_cond >= goal_num_gc)
        {
            cout<<"Goal founded!" << endl;
            cout<<"Node State:"<< endl;
            path.push_back(actual_node);
            path_action.emplace_back(actual_node->get_applied_action());
            cout<<actual_node->get_condition()<<endl;
            while(!(path.back()->get_parent() == nullptr))
            {
                node* father = path.back()->get_parent();
                auto applied_act = father->get_applied_action();
                path.emplace_back(father);
                path_action.emplace_back(applied_act);
            }
            res.set_cost_of_plan(actual_node->get_g_value());
            res.set_num_states_expanded(close_list.size());
            return path_action;
        }
        for(Action a:available_act) //FOr all available actions get feasible actions from the actual_node
        {
            list<GroundedAction> feasible_actions = gen_fsble_actions(env,all_comb,a,actual_gc);
            for(auto it = feasible_actions.begin(); it!=feasible_actions.end();++it) //Insert every feasible action into the open list
            {
                successor_condition = execute_action(env,a,*it,actual_gc);
                node* succ_node = new node(successor_condition,GroundedAction("null",list<string> {"A"}));
                string succ_string_gc = succ_node->get_condition();
                double g_val = actual_node->get_g_value() + 1;
                if(close_list.find(succ_node)!=close_list.end())
                    continue;
                else
                {
                    if(open_ptr_track.find(succ_string_gc) != open_ptr_track.end())
                    {
                        if(open_ptr_track[succ_string_gc] > g_val)
                        {
                            succ_node->set_g(g_val);
                        }
                    }
                    else
                    {
                        // double h_val =goal_num_gc-getMatchedConditions(succ_node,goal_condition);
                        double h_val = 0;
                        succ_node->set_g(g_val);
                        succ_node->set_h(h_val);
                        succ_node->set_f();
                        succ_node->set_parent(actual_node);
                        succ_node->set_applied_action(*it);
                        open_ptr.push(succ_node);
                        open_ptr_track.emplace(succ_string_gc,g_val);
                    }
                }
            }
        }

    }
    cout<<"No Path Founded"<<endl;
    return path_action;
}
//------------------Code added by AF above

list<GroundedAction> planner(Env* env,results& res)
{
    //Compute path using A*
    list<GroundedAction> node_path = ComputeSymbolicAstar(env,res);
    node_path.pop_back();
    node_path.reverse();
    return node_path;
}

int main(int argc, char* argv[])
{
    // DO NOT CHANGE THIS FUNCTION
    char* filename = (char*)("example.txt");
    if (argc > 1)
        filename = argv[1];

    cout << "Environment: " << filename << endl << endl;
    Env* env = create_env(filename);
    if (print_status)
    {
        cout << *env;
    }
    auto start_time = std::chrono::system_clock::now();
    results res = results();
    list<GroundedAction> actions = planner(env,res);
    auto end_time = std::chrono::system_clock::now();
    auto time_delay = std::chrono::duration_cast<std::chrono::nanoseconds> (end_time-start_time);
    float time_passed = time_delay.count()*1e-9;
    res.set_planning_time(time_passed);
    cout << "\nPlan: " << endl;
    for (GroundedAction gac : actions)
    {
        cout << gac << endl;
    }
    cout<<"Planning time: "<<res.get_planning_time()<< " seconds" << endl;
    cout<<"Cost of plan: "<<res.get_cost_of_plan()<< endl;
    cout<<"States expanded in search: "<< res.get_states_expanded() << endl;
    delete env; 

    return 0;
}