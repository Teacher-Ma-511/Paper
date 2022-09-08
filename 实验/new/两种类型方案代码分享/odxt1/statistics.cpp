#include<bits/stdc++.h>
using namespace std;
bool cmp(pair<string,int> &a ,pair<string,int> &b){
    return a.second>b.second;
}
int main(){
    std::unordered_map<std::string, std::vector<std::string>> Kid;
    ifstream fin("/home/helen/sample/dataset/clear_30.txt", std::ios::in);
    //ofstream file("/home/helen/sample/dataset/medium.txt",std::ios::out);
    string keyword, id, cd;
    clock_t start = clock();
    for (int i = 0; i < 320000; i++)
    {
        getline(fin,cd);
        //cout << cd << endl;
        //break;
        id = cd.substr(0, 8);
        //file << id << " ";
        for (int j = 8; j < cd.size(); j++)
        {   
            if( (cd[j]>='a' && cd[j]<='z') || (cd[j]>='A' && cd[j]<='Z') ||(cd[j]>='0' && cd[j]<='9')){
                keyword += cd[j];
            }else{
                if(keyword==""){
                    continue;
                }
                Kid[keyword].push_back(id);
               //file << keyword << " ";
                keyword = "";
            }
        }
        //file << '\n';
        //file <<"chenhailong"<< '\n';
        //Kid["chenhailong"].push_back(id);
    }
    fin.close();
    clock_t end = clock();
    std::cout << "time : " << ((double)end - start) / CLOCKS_PER_SEC << "s\n";
    long long sum = 0;
    // for (int i = 36000; i < 100000;i++){
    //     c++;
    //     Kid["chenhailong"].push_back(to_string(c));
    //     file << to_string(c) << " "
    //          << "chenhailong" << '\n';
    // }
    for (auto it : Kid)
    {
        sum += it.second.size();
    }
    std::cout<<"键值对个数："<<sum<<endl;
    std::cout << "关键字个数"<<Kid.size() << endl;
    //std::cout << Kid["chenhailong"].size() << endl;
    ofstream file("/home/helen/sample/dataset/new_medium.txt",std::ios::out);
    vector<pair<string, int>> s;
    for (auto it : Kid)
    {
        s.push_back(make_pair(it.first, it.second.size()));
    }
    sort(s.begin(),s.end(),cmp);
    for (auto it : s)
    {
        file<<it.first<<" "<<it.second<<endl;
    }
    file.close();
    //file.close();
    return 0;
}
