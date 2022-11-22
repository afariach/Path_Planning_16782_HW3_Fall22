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

// template<typename T> std::vector<std::vector<T>> subsets(int n, int r)
//     {
//     std::vector<bool> v(n);
//     std::vector
//     std::fill(v.begin(), v.begin() + r, true);

//     do  {
//         std::vector<int> p;
//         for (int i = 0; i < n; ++i) 
//             if (v[i]) 
//                 p.push_back(i);
//         do  {
//             std::cout << " { ";
//             for(auto i : p)
//                 std::cout << i << ' ';
//             std::cout << "} ";
//             } while(std::next_permutation(p.begin(), p.end()));
//         } while (std::prev_permutation(v.begin(), v.end()));
//     }

// void subsets(int n)
//     {
//     for(int r=0; r<=n; ++r)
//         subsets(n, r);
//     }