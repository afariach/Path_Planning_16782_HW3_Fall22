/*
* Function return all possible combinations of k elements from N-size inputVector.
* The result is returned as a vector of k-long vectors containing all combinations.
*/
template<typename T> std::vector<std::vector<T>> getAllCombinations(const std::vector<T>& inputVector, int k)
{
    std::vector<std::vector<T>> combinations;
    std::vector<int> selector(inputVector.size());
    std::fill(selector.begin(), selector.begin() + k, 1);

    do {
        std::vector<int> selectedIds;
        std::vector<T> selectedVectorElements;
        for (int i = 0; i < inputVector.size(); i++)
            if (selector[i]) 
                selectedIds.push_back(i);
        do
        { 
            for (auto& id : selectedIds) 
                selectedVectorElements.push_back(inputVector[id]);
            combinations.push_back(selectedVectorElements);
            selectedVectorElements.clear();
        }while(next_permutation(selectedIds.begin(),selectedIds.end()));
    } while (std::prev_permutation(selector.begin(), selector.end()));

    return combinations;
}

template<typename T> void print_vector(const std::vector<T>& inputVector)
{
    for(int i = 0; i < inputVector.size(); ++i)
    {
        std::cout<<inputVector[i] <<", ";
    }
    std::cout<<std::endl;
}

class results
{
    private:
        double time;
        double cost_of_plan;
        int expanded_states;
    public:
        results(double time,double plan,int states)
        :time{time},cost_of_plan{plan},expanded_states{states}{};
        results()
        :time{0},cost_of_plan{0},expanded_states{0}{};
        void set_planning_time(double time)
        {
            this->time = time;
        }
        void set_cost_of_plan(double cost_plan)
        {
            this->cost_of_plan = cost_plan;
        }
        void set_num_states_expanded(int states_expand)
        {
            this->expanded_states = states_expand;
        }
        double get_planning_time()
        {
            return this->time;
        }
        double get_cost_of_plan()
        {
            return this->cost_of_plan;
        }
        int get_states_expanded()
        {
            return this->expanded_states;
        }
};
