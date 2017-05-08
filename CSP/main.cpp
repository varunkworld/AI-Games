#include <iostream>
#include <map>
#include <queue>
#include <climits>
#include <algorithm>
#include <time.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <fstream>
using namespace std;

class CSP;
bool AC3(CSP, queue<pair<string, string> >);

class CSP {
    public :
    vector<string> vars;
    map<string, vector<string> > curr_domains;
    map<string, vector<string> > domains;
    map<string, vector<string> > neighbors;
    map<string, string> assignment;
    int nassigns;
    bool fc;
    bool mac;
    bool mcv;
    bool lcv;
    CSP(vector<string> vars, map<string, vector<string> > domains, map<string, vector<string> > neighbors) {
        this->vars = vars;
        this->domains = domains;
        this->curr_domains = domains;
        this->neighbors = neighbors;
    }
    
    bool assign(string var, string val, map<string, string> &assignment) {
        nassigns += 1;
        assignment[var] = val;
        bool valid = true;
        if (curr_domains.size() != 0) {
            if (mac) {
                queue<pair<string, string> > pairs;
                for(string Xk : neighbors[var]) {
                    if (assignment.find(Xk) == assignment.end())
                        pairs.push(make_pair(Xk, var));
                }
                valid = AC3(*this, pairs);
            }
            else if (fc) {
                valid = forward_check(var, val, assignment);
            }
        }
        return valid;
    }
    
    void unassign(string var, map<string, string> &assignment) {
        if (assignment.find(var) != assignment.end()) {
            assignment.erase(var);
            nassigns -= 1;
        }
    }
    
    int nconflicts(string var, string val, map<string, string> assignment) {
        int count = 0;
        for(string var2 : neighbors[var]) {
            count += (assignment.find(var2) != assignment.end() && val == assignment[var2]);
        }
        return count;
    }
    
    bool forward_check(string var, string val, map<string, string> &assignment) {
        //"Do forward checking (current domain reduction) for this assignment."
        if (curr_domains.size() != 0) {
            //# Restore prunings from previous value of var
            //for (B, b) in self.pruned[var]:
            //self.curr_domains[B].append(b)
            //self.pruned[var] = []
            //# Prune any other B=b assignement that conflict with var=val
            for(string B : neighbors[var]) {
                if(assignment.find(B) == assignment.end()) {
                    for(string b : curr_domains[B]) {
                        if(val == b) {
                            curr_domains[B].erase(remove(curr_domains[B].begin(),curr_domains[B].end(),b), curr_domains[B].end());
                            //self.pruned[var].append((B, b))
                        }
                    }
                    if (curr_domains[B].size() == 0)
                        return false;
                }
            }
            return true;
        }
        return false;
    }
    
    vector<string> conflicted_vars(map<string, string> current) {
        vector<string> conflicts;
        for(string var : vars) {
            if(nconflicts(var, current[var], current) > 0) {
                conflicts.push_back(var);
            }
        }
        return conflicts;
    }
};



map<string,string> recursive_backtracking(map<string, string>, CSP);
vector<string> order_domain_values(string, map<string, string>, CSP);
string select_unassigned_variable(map<string, string>, CSP);

map<string, string> backtracking_search(CSP csp, bool mcv = false, bool lcv = false, bool fc = false, bool mac = false) {
    
    csp.mcv = mcv;
    csp.lcv = lcv;
    csp.fc = fc;
    csp.mac = mac;
    
    map<string, string> temp;
    return recursive_backtracking(temp, csp);
}

map<string,string> recursive_backtracking(map<string, string> assignment, CSP csp) {
    if(assignment.size() == csp.vars.size()) {
        return assignment;
    }
    map<string, vector<string> > temp_domains;
    string var = select_unassigned_variable(assignment, csp);
    bool valid;
    for(string val : order_domain_values(var, assignment, csp)) {
        temp_domains = csp.curr_domains;
        if(csp.nconflicts(var, val, assignment) == 0) {
            valid = csp.assign(var, val, assignment);
            if (valid) {
                map<string, string> result = recursive_backtracking(assignment, csp);
                if(result.size() != 0) {
                    return result;
                }
            }
        }
        csp.unassign(var, assignment);
        csp.curr_domains = temp_domains;
    }
    map<string, string> temp;
    return temp;
}

string select_unassigned_variable(map<string, string> assignment, CSP csp) {
    if(csp.mcv) {
        string ret;
        vector<string> arr;
        int min_size = INT_MAX;
        for(string v : csp.vars) {
            if(assignment.find(v) == assignment.end()) {
                if(csp.curr_domains[v].size() < min_size) {
                    min_size = csp.curr_domains[v].size();
                    //ret = v;
                }
            }
        }
        
        for(string v : csp.vars) {
            if(assignment.find(v) == assignment.end() && csp.curr_domains[v].size() == min_size) {
                arr.push_back(v);
            }
        }
        int max_size = INT_MIN;
        for(string v : arr) {
            if(int(csp.neighbors[v].size()) > max_size) {
                max_size = csp.neighbors[v].size();
                ret = v;
            }
        }
        
        return ret;
    } else {
        for(string v : csp.vars) {
            if(assignment.find(v) == assignment.end()) {
                return v;
            }
        }
    }
    return "";
}

vector<string> order_domain_values(string var, map<string, string> assignment, CSP csp) {
    vector<string> domain = csp.curr_domains[var];
    if(csp.lcv) {
        for(int i = 0; i < domain.size(); i++) {
            for(int j = i+1; j < domain.size(); j++) {
                if(csp.nconflicts(var, domain[i], assignment) > csp.nconflicts(var, domain[j], assignment)) {
                    string temp = domain[i];
                    domain[i] = domain[j];
                    domain[j] = temp;
                }
            }
        }
    }
    return domain;
}

bool remove_inconsistent_values(CSP, string, string);

bool AC3(CSP csp, queue<pair<string, string> > q) {
    
    
    while(!q.empty()) {
        pair<string, string> p = q.front();
        q.pop();
        if(remove_inconsistent_values(csp, p.first, p.second)) {
            if(csp.curr_domains[p.first].size() == 0)
                return false;
            for(string k : csp.neighbors[p.first]) {
                if(k != p.second)
                    q.push(make_pair(k, p.first));
            }
        }
    }
    return true;
}

bool remove_inconsistent_values(CSP csp, string Xi, string Xj) {
    bool removed = false;
    for(string x : csp.curr_domains[Xi]) {
        bool check = true;
        for(string y : csp.curr_domains[Xj]) {
            if(x != y) {
                check = false;
            }
        }
        if(check) {
            csp.curr_domains[Xi].erase(remove(csp.curr_domains[Xi].begin(), csp.curr_domains[Xi].end(), x), csp.curr_domains[Xi].end());
            removed = true;
        }
    }
    return removed;
}

string min_conflicts_value(CSP csp, string var, map<string, string> current) {
    int min = INT_MAX;
    string min_val = "";
    for(string val : csp.curr_domains[var]) {
        if(csp.nconflicts(var, val, current) < min) {
            min = csp.nconflicts(var, val, current);
            min_val = val;
        }
    }
    return min_val;
}
map<string,string> min_conflicts(CSP csp, int max_steps = 1000) {
    map<string, string> current;
    for(string var : csp.vars) {
        string val = min_conflicts_value(csp, var, current);
        csp.assign(var, val, current);
    }
    
    srand (time(NULL));
    
    for(int i = 0; i < max_steps; i++) {
        vector<string> conflicted = csp.conflicted_vars(current);
        if(conflicted.size() == 0) {
            return current;
        }
        string var = conflicted[rand() % conflicted.size()];
        string val = min_conflicts_value(csp, var, current);
        csp.assign(var, val, current);
    }
    map<string, string> temp;
    return temp;
}

CSP ramdomly_generate_graph(int m, int n, int e, vector<string> color) {
    vector<string> vars;
    map<string,vector<string> > neighbours;

    map<string, vector<string> > domains;
    for(int i = 1; i <= (m*n); i++) {
        vars.push_back(to_string(i));
        vector<string> temp;
        neighbours[to_string(i)] = temp;
        domains[to_string(i)] = color;
    }
    
    srand (time(NULL));
    vector<pair<string,string> > done;
    int i = 0;
    ofstream f("Random_Graph.txt");

    while(i < e){
        int v1 = (rand() % (m*n)) + 1;
        int v2 = (rand() % (m*n)) + 1;
        if( v2 == (v1-1) || v2 == (v1+1) || v2 == (v1-m) || v2 == (v1+m) || v2 == (v1+m-1) || v2 == (v1=m+1)) {
            string var1 = to_string(v1);
            string var2 = to_string(v2);
            if(find(done.begin(),done.end(), make_pair(var1, var2)) == done.end()) {
                i++;
                done.push_back(make_pair(var1, var2));
                f << var1 <<" " << var2 << endl;
                neighbours[var1].push_back(var2);
                neighbours[var2].push_back(var1);
            }
        }
    }
    f.close();
    return CSP(vars, domains, neighbours);
}

void print(map<string, string > ret) {
        for (const auto &p : ret) {
            cout << "m[" << p.first << "] = " << p.second << '\n';
        }
}
void output(CSP graph, string t){
    struct timespec tstart={0,0}, tend={0,0};
    map<string, string> ret;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    ret = backtracking_search(graph, true, true, false, false);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    string empty = "found";
    
    ofstream f;
    if(ret.empty()) {
        empty = "not found";
    } else {
        f = ofstream("sbt"+t+"_Assignment.txt");
        for (const auto &p : ret) {
            f << p.first << " " << p.second << '\n';
        }
        f.close();
    }
    cout << "simple backtracking time (" + empty + ") : " << ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec) << " secs" << endl;
    graph.curr_domains = graph.domains;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    ret = backtracking_search(graph, true, true, true, false);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    empty = "found";
    if(ret.empty()) {
        empty = "not found";
    } else {
        f = ofstream("bft"+t+"_Assignment.txt");
        for (const auto &p : ret) {
            f << p.first << " " << p.second << '\n';
        }
        f.close();
    }
    cout << "backtracking with FC time (" + empty + ") : "  << ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec) << " secs" << endl;

    graph.curr_domains = graph.domains;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    ret = backtracking_search(graph, true, true, false, true);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    empty = "found";
    if(ret.empty()) {
        empty = "not found";
    }  else {
        f = ofstream("bmt"+t+"_Assignment.txt");
        for (const auto &p : ret) {
            f << p.first << " " << p.second << '\n';
        }
        f.close();
    }
    //print(ret);
    cout << "backtracking with MAC time (" + empty + ") : "  << ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec) << " secs" << endl;

    graph.curr_domains = graph.domains;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    ret = min_conflicts(graph);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    empty = "found";
    if(ret.empty()) {
        empty = "not found";
    } else {
        f = ofstream("mct"+t+"_Assignment.txt");
        for (const auto &p : ret) {
            f << p.first << " " << p.second << '\n';
        }
        f.close();
    }
    cout << "min-conflicts time (" + empty + ") : "  << ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec) << " secs" << endl;
}

int main() {

     vector<string> vars1 = {"SA", "NT", "NSW", "T", "WA", "Q", "V"};
     vector<string> color1 = {"R", "G", "B"};
     map<string, vector<string> > domains1;
     domains1["SA"] = color1;
     domains1["NT"] = color1;
     domains1["NSW"] = color1;
     domains1["T"] = color1;
     domains1["WA"] = color1;
     domains1["Q"] = color1;
     domains1["V"] = color1;
     map<string, vector<string> > neighbours1;
     neighbours1["SA"] = {"WA", "NT", "Q", "NSW", "V"};
     neighbours1["NT"] = {"WA", "Q", "SA"};
     neighbours1["NSW"] = {"Q", "V", "SA"};
     neighbours1["T"] = {};
     neighbours1["WA"] = {"NT", "SA"};
     neighbours1["Q"] = {"NT", "SA", "NSW"};
     neighbours1["V"] = {"SA", "NSW"};
    ofstream f("Australia_Graph.txt");
    for (const auto &p : neighbours1) {
        for(string s : p.second) {
            f << p.first << " " << s << '\n';
        }
    }
    f.close();
    CSP graph1 = CSP(vars1, domains1, neighbours1);
    cout << "Australia : \n";
    output(graph1, "_Australia");
    
    vector<string> vars2 = {"YT", "NT", "BC", "AB", "SK", "NV", "MB", "ON", "QC", "NF", "NB", "PE", "NS", "LB"};
    vector<string> color2 = {"R", "G", "B"};
    map<string, vector<string> > domains2;
    domains2["YT"] = color2;
    domains2["NT"] = color2;
    domains2["BC"] = color2;
    domains2["AB"] = color2;
    domains2["SK"] = color2;
    domains2["NV"] = color2;
    domains2["MB"] = color2;
    domains2["ON"] = color2;
    domains2["QC"] = color2;
    domains2["NF"] = color2;
    domains2["NB"] = color2;
    domains2["PE"] = color2;
    domains2["NS"] = color2;
    domains2["LB"] = color2;
    map<string, vector<string> > neighbours2;
    neighbours2["YT"] = {"BC", "NT"};
    neighbours2["NT"] = {"YT", "BC", "AB", "SK", "NV"};
    neighbours2["BC"] = {"YT", "NT", "AB"};
    neighbours2["AB"] = {"BC", "NT", "SK"};
    neighbours2["SK"] = {"NT", "AB", "MB"};
    neighbours2["NV"] = {"NT", "MB"};
    neighbours2["MB"] = {"SK", "NV", "ON"};
    neighbours2["ON"] = {"MB", "QC"};
    neighbours2["QC"] = {"ON", "NF", "LB", "PE", "NB"};
    neighbours2["NF"] = {"QC", "LB"};
    neighbours2["NB"] = {"QC", "NS"};
    neighbours2["PE"] = {"QC", "LB", "NS"};
    neighbours2["NS"] = {"NB", "PE", "LB"};
    neighbours2["LB"] = {"NF", "QC", "PE", "NS"};
    f = ofstream("Canada_Graph.txt");
    for (const auto &p : neighbours2) {
        for(string s : p.second) {
            f << p.first << " " << s << '\n';
        }
    }
    f.close();
    CSP graph2 = CSP(vars2, domains2, neighbours2);
    cout << "\n\nCanada : \n";
    output(graph2, "_Canada");
    vector<string> c ={"R","G","B","Y"};
    for(int i = 1; i <= 1; i++) {
        cout << "gfsgsd\n";
        CSP graph3 = ramdomly_generate_graph(i, i, 1.5*(i*i),c);
        
        cout << "\n\nRandom Graph: \n";
        output(graph3, "_Random");
        
    }

    return 0;
}

