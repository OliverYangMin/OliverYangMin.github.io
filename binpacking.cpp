#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
// #include <boost/python/numpy.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
// #include <set>

using namespace std;
namespace bp = boost::python;
// namespace bn = boost::python::numpy;

class RECT
{
    public:
    RECT(){};
    RECT(string bin_id,int x,int y,int width,int length){
        this->bin_id = bin_id;
        this->x = x;
        this->y = y;
        this->width = width;
        this->length = length;
    }

    bool is_contained(const RECT& obj)
    {
        return (x>=obj.x && y>=obj.y && x+width<=obj.x+obj.width && y+length<=obj.y+obj.length);
    }

    string bin_id;
    int x,y,width,length;
};

bool operator==(const RECT& rect1,const RECT& rect2) {
    return (rect1.x==rect2.x && rect1.y==rect2.y && rect1.width==rect2.width && rect1.length==rect2.length);
}
class BIN
{
    public:
    BIN(){};
    BIN(string bin_id,int bin_weight,int bin_length,int bin_width,string station)
    {
        this->bin_id = bin_id;
        this->bin_weight = bin_weight;
        this->bin_length = bin_length;
        this->bin_width = bin_width;
        this->station = station;
    }

    string bin_id,station;
    int bin_weight,bin_length,bin_width;

};

class BinCounter
{
    public:
    BinCounter(){};
    BinCounter(int bin_length,int bin_width)
    {
        this->bin_length = bin_length;
        this->bin_width = bin_width;
        this->num = 0;
    }

    void add_one()
    {
        num += 1;
    }
    int bin_length,bin_width,num;
};

bool operator==(const BIN& bin1,const BIN& bin2)
{
    return (bin1.bin_id==bin2.bin_id);
}

int cmp(const pair<int, int>& x, const pair<int, int>& y)  
{  
    return x.second > y.second;  
}

int cmp_counter(const BinCounter& x,const BinCounter& y)
{
    return x.num > y.num;
}

bool in_vector(vector<string> v,string element)
{
    vector<string>::iterator it;
    it = find(v.begin(),v.end(),element);
    if (it!=v.end()){
        return true;
    }
    else
    {
        return false;
    }
    
}
class Data
{
    public:
    Data(){};
    Data(RECT virtual_rect,RECT filled_rect,int best_short_left,int best_long_left)
    {
        this->virtual_rect = virtual_rect;
        this->filled_rect = filled_rect;
        this->best_short_left = best_short_left;
        this->best_long_left = best_long_left;
    }
    // Data(RECT& rect1, RECT& rect2,int short_left,int long_left):virtual_rect(rect1),filled_rect(rect2),best_short_left(short_left),best_long_left(long_left)
    // {}
    RECT virtual_rect, filled_rect;
    int best_short_left,best_long_left;

};

class KnapData
{
    public:
    KnapData(){};
    KnapData(int value,vector<int> position)
    {
        this->value = value;
        this->position = position;
    }
    int value;
    vector<int> position;

};

class BinGroup
{
    public:
    BinGroup(){};
    BinGroup(vector<BIN> boxs,vector<int> box_width)
    {
        this->boxs = boxs;
        this->box_width = box_width;
    }
    vector<BIN> boxs;
    vector<int> box_width;
};

class Group
{
    public:
    Group(){};
    Group(vector<BIN> boxs,RECT virtual_rect,vector<RECT> actual_rects)
    {
        this->boxs = boxs;
        this->virtual_rect = virtual_rect;
        this->actual_rects = actual_rects;
    }
    vector<BIN> boxs;
    RECT virtual_rect;
    vector<RECT> actual_rects;
};

class Vehicle
{
    public:
    Vehicle(string vehicle_id,int width,int length,int weight,bool allow_flip)
    {
        this->vehicle_id = vehicle_id;
        this->width = width;
        this->length = length;
        this->weight = weight;
        this->allow_flip= allow_flip;
        Init();

    }

    void Init()
    {
        RECT empty_rect;
        empty_rect = RECT("Empty_Veh",0,0,this->width,this->length);
        used_rects.clear();
        free_rects.clear();
        inserted_bins.clear();
        free_rects.push_back(empty_rect);
    }
    void update_vehicle(const Vehicle& veh)
    {
        vehicle_id = veh.vehicle_id;
        width = veh.width;
        length = veh.length;
        weight = veh.weight;
        allow_flip= veh.allow_flip;
        used_rects = veh.used_rects;
        free_rects = veh.free_rects;
        inserted_bins = veh.inserted_bins; 
    }

    Data find_position_for_a_bin(const BIN& temp_bin)
    {
        RECT virtual_rect,filled_rect,tmp_rect;
        int best_short_left = INT_MAX;
        int best_long_left = INT_MAX;
        Data package;
        for (size_t i=0;i<free_rects.size();i++)
        {
            tmp_rect = free_rects[i];
            if (tmp_rect.width>=temp_bin.bin_width && tmp_rect.length>=temp_bin.bin_length)
            {
                int left_over_x = abs(tmp_rect.width - temp_bin.bin_width);
                int left_over_y = abs(tmp_rect.length - temp_bin.bin_length);
                int short_left = min(left_over_x,left_over_y);
                int long_left = max(left_over_x,left_over_y);
                if (short_left < best_short_left || (short_left == best_short_left && long_left < best_long_left))
                {
                    virtual_rect = RECT(temp_bin.bin_id,tmp_rect.x,tmp_rect.y,temp_bin.bin_width,temp_bin.bin_length);
                    best_short_left = short_left;
                    best_long_left = long_left;
                    filled_rect = tmp_rect;
                }
            }
            if (allow_flip && tmp_rect.width >= temp_bin.bin_length && tmp_rect.length >= temp_bin.bin_width)
            {
                int left_over_x = abs(tmp_rect.width - temp_bin.bin_length);
                int left_over_y = abs(tmp_rect.length - temp_bin.bin_width);
                int short_left = min(left_over_x,left_over_y);
                int long_left = max(left_over_x,left_over_y);
                if (short_left < best_short_left || (short_left == best_short_left && long_left < best_long_left))
                {
                    virtual_rect = RECT(temp_bin.bin_id,tmp_rect.x,tmp_rect.y,temp_bin.bin_length,temp_bin.bin_width);
                    best_short_left = short_left;
                    best_long_left = long_left;
                    filled_rect = tmp_rect;
                }
            }
        }
        package = Data(virtual_rect,filled_rect,best_short_left,best_long_left);
        return package;
    }

    vector<RECT> split_rect_by_rect(const RECT& space_rect,const RECT& bin_rect)
    {
        vector<RECT> maximal_space_rect_list;
        RECT tmp_rect;
        if (bin_rect.x >= space_rect.x + space_rect.width || bin_rect.x + bin_rect.width <= space_rect.x || bin_rect.y >= space_rect.y + space_rect.length || bin_rect.y + bin_rect.length <= space_rect.y)
        {
            return maximal_space_rect_list;
        }
        if (bin_rect.x < space_rect.x + space_rect.width && bin_rect.x + bin_rect.width > space_rect.x)
        {
            if (bin_rect.y > space_rect.y)
            {
                tmp_rect = RECT("Empty_Con",space_rect.x,space_rect.y,space_rect.width,bin_rect.y - space_rect.y);
                maximal_space_rect_list.push_back(tmp_rect);
            }
            if (bin_rect.y + bin_rect.length < space_rect.y + space_rect.length)
            {
                tmp_rect = RECT("Empty_Con",space_rect.x,bin_rect.y+ bin_rect.length,space_rect.width,space_rect.y + space_rect.length - (bin_rect.y+ bin_rect.length));
                maximal_space_rect_list.push_back(tmp_rect);
            }
        }
        if (bin_rect.y < space_rect.y + space_rect.length && bin_rect.y + bin_rect.length > space_rect.y)
        {
            if (bin_rect.x > space_rect.x)
            {
                tmp_rect = RECT("Empty_Con",space_rect.x,space_rect.y,bin_rect.x - space_rect.x,space_rect.length);
                maximal_space_rect_list.push_back(tmp_rect);
            }
            if (bin_rect.x + bin_rect.width < space_rect.x + space_rect.width)
            {
                tmp_rect = RECT("Empty_Con",bin_rect.x+ bin_rect.width,space_rect.y,space_rect.x + space_rect.width - (bin_rect.x + bin_rect.width),space_rect.length);
                maximal_space_rect_list.push_back(tmp_rect);
            }
        }
        if (maximal_space_rect_list.empty())
        {
            tmp_rect = RECT("zero_space",bin_rect.x,bin_rect.y,0,0);
            maximal_space_rect_list.push_back(tmp_rect);
        }
        return maximal_space_rect_list;
    }
    void place_rect(RECT& bin_rect)
    {
        vector<RECT> newly_generated_maximal_space;
        vector<RECT> new_maximal_space;
        RECT free_rect;
        for (size_t i=0;i<free_rects.size();i++)
        {
            free_rect = free_rects[i];
            new_maximal_space = split_rect_by_rect(free_rect,bin_rect);
            if (!new_maximal_space.empty())
            {
                free_rects.erase(free_rects.begin()+i);
                i--;
                for (size_t j=0;j<new_maximal_space.size();j++){
                    newly_generated_maximal_space.push_back(new_maximal_space[j]);
                }
            }
        }

        for (size_t i=0;i<newly_generated_maximal_space.size();i++)
        {
            free_rects.push_back(newly_generated_maximal_space[i]);
        }


        RECT rect1,rect2;
        for (size_t i=0;i<free_rects.size();i++)
        {
            for (size_t j=i+1;j<free_rects.size();j++){
                rect1 = free_rects[i];
                rect2 = free_rects[j];
                if (rect1.is_contained(rect2))
                {
                    free_rects.erase(free_rects.begin()+i);
                    i--;
                    break;
                }
                if (rect2.is_contained(rect1))
                {
                    free_rects.erase(free_rects.begin()+j);
                    j--;
                }
            }
        }

        // for (size_t i=0;i<newly_generated_maximal_space.size();i++)
        // {
        //     free_rects.push_back(newly_generated_maximal_space[i]);
        // }
    }

    bp::list insert_bins_into_vehicle(bp::list& bin_list)
    {
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }

        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            int best_area = 0;
            bool insert_mark = false;
            Data package;
            BIN temp_bin;
            RECT best_rect;
            BIN ready_to_insert_bin;
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (temp_bin.bin_weight > weight) continue;
                int bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;

            weight -= ready_to_insert_bin.bin_weight;
            inserted_bins.push_back(ready_to_insert_bin);
            place_rect(best_rect);
            used_rects.push_back(best_rect);

            for (size_t i=0;i<bin_vec.size();i++)
            {
                if (bin_vec[i]==ready_to_insert_bin)
                {
                    bin_vec.erase(bin_vec.begin()+i);
                    break;
                }
            }
        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }

        return left_bin_list;
    }

bp::list insert_big_bins_first(bp::list& bin_list)
    {
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }

        int best_area = 0;
        int bin_area;
        RECT best_rect;
        BIN ready_to_insert_bin;
        BIN temp_bin;
        for (size_t i=0;i<bin_vec.size();i++)
        {
            temp_bin = bin_vec[i];
            bin_area = temp_bin.bin_length * temp_bin.bin_width;
            if (bin_area > best_area)
            {
                best_area = bin_area;
                best_rect = RECT(temp_bin.bin_id,0,0,temp_bin.bin_width,temp_bin.bin_length);
                ready_to_insert_bin = temp_bin;
            }
        }

        weight -= ready_to_insert_bin.bin_weight;
        inserted_bins.push_back(ready_to_insert_bin);
        place_rect(best_rect);
        used_rects.push_back(best_rect);

        for (size_t i=0;i<bin_vec.size();i++)
        {
            if (bin_vec[i]==ready_to_insert_bin)
            {
                bin_vec.erase(bin_vec.begin()+i);
                break;
            }
        }

        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            best_area = 0;
            bool insert_mark = false;
            Data package;
            
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (temp_bin.bin_weight > weight) continue;
                bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;

            weight -= ready_to_insert_bin.bin_weight;
            inserted_bins.push_back(ready_to_insert_bin);
            place_rect(best_rect);
            used_rects.push_back(best_rect);

            for (size_t i=0;i<bin_vec.size();i++)
            {
                if (bin_vec[i]==ready_to_insert_bin)
                {
                    bin_vec.erase(bin_vec.begin()+i);
                    break;
                }
            }
        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }

        return left_bin_list;
    }
    bp::list insert_bins_by_pattern(bp::list& bin_list,bp::list& pattern)
    {
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }

        vector<RECT> pattern_vec;
        for (size_t i=0;i<len(pattern);i++)
        {
            pattern_vec.push_back(bp::extract<RECT>(pattern[i]));
        }

        BIN temp_bin;
        RECT temp_pattern;
        RECT best_rect;
        BIN ready_to_insert_bin;
        int x_coordinate = 0;
        for (size_t i=0;i<pattern_vec.size();i++)
        {
            temp_pattern = pattern_vec[i];
            for (size_t j=0;j<bin_vec.size();j++)
            {
                temp_bin = bin_vec[j];
                if ((temp_pattern.length==temp_bin.bin_length && temp_pattern.width==temp_bin.bin_width) || \
                (temp_pattern.length==temp_bin.bin_width &&temp_pattern.width == temp_bin.bin_length))
                {
                    best_rect = RECT(temp_bin.bin_id,x_coordinate,0,temp_pattern.width,temp_pattern.length);
                    ready_to_insert_bin = temp_bin;
                    weight -= ready_to_insert_bin.bin_weight;
                    inserted_bins.push_back(ready_to_insert_bin);
                    place_rect(best_rect);
                    used_rects.push_back(best_rect);

                    bin_vec.erase(bin_vec.begin()+j);

                    break;
                }
            }
            x_coordinate += temp_pattern.width;
        }

        int best_area = 0;
        int bin_area;
        
        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            best_area = 0;
            bool insert_mark = false;
            Data package;
            
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (temp_bin.bin_weight > weight) continue;
                bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;

            weight -= ready_to_insert_bin.bin_weight;
            inserted_bins.push_back(ready_to_insert_bin);
            place_rect(best_rect);
            used_rects.push_back(best_rect);

            for (size_t i=0;i<bin_vec.size();i++)
            {
                if (bin_vec[i]==ready_to_insert_bin)
                {
                    bin_vec.erase(bin_vec.begin()+i);
                    break;
                }
            }
        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }

        return left_bin_list;
    }

    bp::list insert_bins_by_bottom_first(bp::list& bin_list,int tolerance)
    {
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }

        vector<BinCounter> size_list;
        BinCounter tempcounter;
        vector<pair<int,int> > cheap_bin_vec;
        vector<pair<int,int> > scarse_bin_vec;
        size_list = sort_bins_type_by_counter(bin_vec);
        for (size_t i=0;i<size_list.size();i++)
        {
            tempcounter = size_list[i];
            cout << "length:"<<tempcounter.bin_length<<"width:"<<tempcounter.bin_width<<"num"<<tempcounter.num<<endl;
            // if (int(length/tempcounter.bin_length) * 2<=tempcounter.num)
            if (2<=tempcounter.num)
            {
                cheap_bin_vec.push_back(make_pair(tempcounter.bin_length,tempcounter.bin_width));
            }
            // if (int(length/tempcounter.bin_length)<=tempcounter.num)
            if (1<=tempcounter.num)
            {
                scarse_bin_vec.push_back(make_pair(tempcounter.bin_length,tempcounter.bin_width));
            }
        }

        cout << "cheap length:"<<cheap_bin_vec.size()<<endl;

        vector<RECT> pattern_vec;
        pattern_vec = get_bottom_pattern(cheap_bin_vec,scarse_bin_vec,tolerance);

        BIN temp_bin;
        RECT temp_pattern;
        RECT best_rect;
        BIN ready_to_insert_bin;
        int x_coordinate = 0;
        for (size_t i=0;i<pattern_vec.size();i++)
        {
            temp_pattern = pattern_vec[i];
            for (size_t j=0;j<bin_vec.size();j++)
            {
                temp_bin = bin_vec[j];
                if ((temp_pattern.length==temp_bin.bin_length && temp_pattern.width==temp_bin.bin_width) || \
                (temp_pattern.length==temp_bin.bin_width &&temp_pattern.width == temp_bin.bin_length))
                {
                    best_rect = RECT(temp_bin.bin_id,x_coordinate,0,temp_pattern.width,temp_pattern.length);
                    ready_to_insert_bin = temp_bin;
                    weight -= ready_to_insert_bin.bin_weight;
                    inserted_bins.push_back(ready_to_insert_bin);
                    place_rect(best_rect);
                    used_rects.push_back(best_rect);

                    bin_vec.erase(bin_vec.begin()+j);

                    break;
                }
            }
            x_coordinate += temp_pattern.width;
        }

        int best_area = 0;
        int bin_area;
        
        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            best_area = 0;
            bool insert_mark = false;
            Data package;
            
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (temp_bin.bin_weight > weight) continue;
                bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;

            weight -= ready_to_insert_bin.bin_weight;
            inserted_bins.push_back(ready_to_insert_bin);
            place_rect(best_rect);
            used_rects.push_back(best_rect);

            for (size_t i=0;i<bin_vec.size();i++)
            {
                if (bin_vec[i]==ready_to_insert_bin)
                {
                    bin_vec.erase(bin_vec.begin()+i);
                    break;
                }
            }
        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }

        return left_bin_list;
    }
    bp::list insert_group_bins_into_vehicle(bp::list& bin_list,int para_num,int tolerance)
    {
        // int para_num = 5;
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }
        vector<string> un_fit_bin_id;
        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            int best_area = 0;
            bool insert_mark = false;
            Data package;
            BIN temp_bin;
            RECT best_rect;
            RECT best_maximal_space;
            BIN ready_to_insert_bin;
            Group current_group;
            // cout << bin_vec.size() << endl;
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (in_vector(un_fit_bin_id,temp_bin.bin_id))continue;     
                int bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    best_maximal_space = package.filled_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;
            // cout << "current rect width:" <<best_maximal_space.width << endl;
            current_group = group_bins_solver(best_maximal_space,bin_vec,para_num,tolerance);
            int group_weight = 0;
            for (size_t i=0;i<current_group.boxs.size();i++)
            {
                group_weight += current_group.boxs[i].bin_weight;
            }
            // cout << group_weight << endl;
            if (group_weight > 0 && group_weight <= weight)
            {
                weight -= group_weight;
                for (size_t i=0;i<current_group.boxs.size();i++)
                {
                    inserted_bins.push_back(current_group.boxs[i]);
                }
                place_rect(current_group.virtual_rect);
                for (size_t i=0;i<current_group.actual_rects.size();i++)
                {
                    used_rects.push_back(current_group.actual_rects[i]);
                }
                for (size_t j=0;j<current_group.boxs.size();j++)
                {
                    for (size_t i=0;i<bin_vec.size();i++)
                    {
                        if (bin_vec[i]==current_group.boxs[j])
                        {
                            bin_vec.erase(bin_vec.begin()+i);
                            break;
                        }
                    }
                }
            }
            else
            {
                if (ready_to_insert_bin.bin_weight <= weight)
                {
                    weight -= ready_to_insert_bin.bin_weight;
                    inserted_bins.push_back(ready_to_insert_bin);
                    place_rect(best_rect);
                    used_rects.push_back(best_rect);
                    for (size_t i=0;i<bin_vec.size();i++)
                    {
                        if (bin_vec[i]==ready_to_insert_bin)
                        {
                            bin_vec.erase(bin_vec.begin()+i);
                            break;
                        }
                    }
                }
                else
                {
                    un_fit_bin_id.push_back(ready_to_insert_bin.bin_id);
                }
                
            }

        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }
        return left_bin_list;
    }

    bp::list insert_group_bins_big_first(bp::list& bin_list,int para_num,int tolerance)
    {
        // int para_num = 5;
        vector<BIN> bin_vec;
        bp::list left_bin_list;
        for (int i = 0; i < len(bin_list); i++)
        {
            bin_vec.push_back(bp::extract<BIN>(bin_list[i]));
        }
        vector<string> un_fit_bin_id;

        int best_area = 0;
        int bin_area;
        RECT best_rect;
        BIN ready_to_insert_bin;
        BIN temp_bin;
        for (size_t i=0;i<bin_vec.size();i++)
        {
            temp_bin = bin_vec[i];
            bin_area = temp_bin.bin_length * temp_bin.bin_width;
            if (bin_area > best_area)
            {
                best_area = bin_area;
                best_rect = RECT(temp_bin.bin_id,0,0,temp_bin.bin_width,temp_bin.bin_length);
                ready_to_insert_bin = temp_bin;
            }
        }

        weight -= ready_to_insert_bin.bin_weight;
        inserted_bins.push_back(ready_to_insert_bin);
        place_rect(best_rect);
        used_rects.push_back(best_rect);

        for (size_t i=0;i<bin_vec.size();i++)
        {
            if (bin_vec[i]==ready_to_insert_bin)
            {
                bin_vec.erase(bin_vec.begin()+i);
                break;
            }
        }
        while (!bin_vec.empty())
        {
            int best_short_left = INT_MAX;
            int best_long_left = INT_MAX;
            best_area = 0;
            bool insert_mark = false;
            Data package;
            RECT best_maximal_space;
            Group current_group;
            // cout << bin_vec.size() << endl;
            for (size_t i=0;i<bin_vec.size();i++)
            {
                temp_bin = bin_vec[i];
                if (in_vector(un_fit_bin_id,temp_bin.bin_id))continue;     
                bin_area = temp_bin.bin_length * temp_bin.bin_width;
                package = find_position_for_a_bin(temp_bin);
                if (package.best_short_left == INT_MAX) continue;
                if (package.best_short_left < best_short_left || \
                (package.best_short_left==best_short_left && package.best_long_left < best_long_left) || \
                (package.best_short_left==best_short_left && package.best_long_left == best_long_left && bin_area >best_area))
                {
                    best_short_left = package.best_short_left;
                    best_long_left = package.best_long_left;
                    best_area = bin_area;
                    insert_mark = true;
                    best_rect = package.virtual_rect;
                    best_maximal_space = package.filled_rect;
                    ready_to_insert_bin = temp_bin;
                }

            }
            if (!insert_mark) break;
            // cout << "current rect width:" <<best_maximal_space.width << endl;
            current_group = group_bins_solver(best_maximal_space,bin_vec,para_num,tolerance);
            int group_weight = 0;
            for (size_t i=0;i<current_group.boxs.size();i++)
            {
                group_weight += current_group.boxs[i].bin_weight;
            }
            // cout << group_weight << endl;
            if (group_weight > 0 && group_weight <= weight)
            {
                weight -= group_weight;
                for (size_t i=0;i<current_group.boxs.size();i++)
                {
                    inserted_bins.push_back(current_group.boxs[i]);
                }
                place_rect(current_group.virtual_rect);
                for (size_t i=0;i<current_group.actual_rects.size();i++)
                {
                    used_rects.push_back(current_group.actual_rects[i]);
                }
                for (size_t j=0;j<current_group.boxs.size();j++)
                {
                    for (size_t i=0;i<bin_vec.size();i++)
                    {
                        if (bin_vec[i]==current_group.boxs[j])
                        {
                            bin_vec.erase(bin_vec.begin()+i);
                            break;
                        }
                    }
                }
            }
            else
            {
                if (ready_to_insert_bin.bin_weight <= weight)
                {
                    weight -= ready_to_insert_bin.bin_weight;
                    inserted_bins.push_back(ready_to_insert_bin);
                    place_rect(best_rect);
                    used_rects.push_back(best_rect);
                    for (size_t i=0;i<bin_vec.size();i++)
                    {
                        if (bin_vec[i]==ready_to_insert_bin)
                        {
                            bin_vec.erase(bin_vec.begin()+i);
                            break;
                        }
                    }
                }
                else
                {
                    un_fit_bin_id.push_back(ready_to_insert_bin.bin_id);
                }
                
            }

        }
        for (size_t i=0;i<bin_vec.size();i++)
        {
            left_bin_list.append(bin_vec[i]);
        }
        return left_bin_list;
    }

    bp::list get_used_rects()
    {
        bp::list used_rects_list;
        for (size_t i=0;i<used_rects.size();i++)
        {
            used_rects_list.append(used_rects[i]);
        }
        return used_rects_list;
    }

    bp::list get_inserted_bins()
    {
        bp::list inserted_bins_list;
        for (size_t i=0;i<inserted_bins.size();i++)
        {
            inserted_bins_list.append(inserted_bins[i]);
        }
        return inserted_bins_list;
    }
    void set_visited_station(bp::list vis_station)
    {
        visited_station = vis_station;
    }

    bp::list get_visited_station()
    {
        return visited_station;
    }
    float calc_loading_ratio()
    {
        float sum_area = 0;
        for (size_t i=0;i<used_rects.size();i++)
        {
            sum_area += used_rects[i].length * used_rects[i].width;
        }
        ratio = sum_area / (width * length);
        return ratio;
    }

    KnapData knapsack_solver(int capacity,vector<int> weights)
    {
        int matrix[weights.size()+1][capacity+1];
        for (size_t j=0;j<capacity+1;j++)
        {
            matrix[0][j] = 0;
        }
        vector<int> index_list;
        int position = weights.size();
        KnapData result;
        for (size_t i=1;i<weights.size()+1;i++)
        {
            int item = weights[i-1];
            for (size_t j=0;j<capacity+1;j++)
            {
                if (item > j)
                {
                    matrix[i][j] = matrix[i-1][j];
                }
                else
                {
                    matrix[i][j] = max(matrix[i-1][j],item + matrix[i-1][j-item]);
                }
                
            }
            if (matrix[i][capacity] == capacity)
            {
                position = i;
                break;
            }

        }
        int j = capacity;
        for (size_t i=position;i>0;i--)
        {
            if (matrix[i][j] != matrix[i-1][j])
            {
                index_list.push_back(i-1);
                j -= weights[i-1];
            }
        }

        result = KnapData(matrix[position][capacity],index_list);
        return result;
    }
    BinGroup detect_bingroup(vector<BIN>& bin_list,int size,int rect_width,int tolerance)
    {
        vector<BIN> relative_bins;
        vector<int> tmp_weights;
        BIN element;
        KnapData result;
        for (size_t i=0;i<bin_list.size();i++)
        {
            element = bin_list[i];
            if (element.bin_length == size)
            {
                relative_bins.push_back(element);
                tmp_weights.push_back(element.bin_width);
            }
            else if (element.bin_width == size)
            {
                relative_bins.push_back(element);
                tmp_weights.push_back(element.bin_length);
            }
            else 
            {
                continue;
            }

        }
        // cout << "relative bin number:"<<tmp_weights.size() << endl;
        // cout << "knapsack rect width: " << rect_width << endl;
        result = knapsack_solver(rect_width,tmp_weights);
        // cout << "rect width:"<< rect_width <<endl;
        // cout << "calculated rect width" <<result.value << endl;
        vector<BIN> bin_pac;
        vector<int> used_width;
        BinGroup result_bin_group;
        if (result.value >= rect_width - tolerance)
        {
            for (size_t i=0;i<result.position.size();i++)
            {
                int position_index = result.position[i];
                bin_pac.push_back(relative_bins[position_index]);
                used_width.push_back(tmp_weights[position_index]);
            }
        }
        result_bin_group = BinGroup(bin_pac,used_width);
        return result_bin_group;
    }

    Group group_bins_solver(RECT& maximal_space,vector<BIN>& bin_vec,int para_num,int tolerance)
    {
        vector<int> freq_list;
        freq_list = freq_sorting(bin_vec);
        vector<BIN> result_bins;
        int mark = 0;
        vector<RECT> actual_rects;
        RECT virtual_rect = RECT("grouped",maximal_space.x,maximal_space.y,0,0);
        BinGroup temp_bin_group;
        int choose_size;
        if (para_num > freq_list.size())
        {
            para_num = freq_list.size();
        }
        for (size_t i=0;i<para_num;i++)
        {
            choose_size = freq_list[i];
            // cout << "choosed size:" << choose_size << endl;
            if (choose_size > maximal_space.length)
            {
                continue;
            }
            temp_bin_group = detect_bingroup(bin_vec,choose_size,maximal_space.width,tolerance);
            if (temp_bin_group.boxs.size() > 0)
            {
                // cout << "bin group size: " <<temp_bin_group.boxs.size() << endl;
                mark = 1;
                break;
            }
        }
        if (mark)
        {
            int virtual_width = 0;
            for (size_t i=0;i<temp_bin_group.box_width.size();i++)
            {
                virtual_width += temp_bin_group.box_width[i];
            }
            virtual_rect = RECT("grouped",maximal_space.x,maximal_space.y,virtual_width,choose_size);
            int move_x = maximal_space.x;
            for (size_t i=0;i<temp_bin_group.box_width.size();i++)
            {
                RECT tmp_rect = RECT(temp_bin_group.boxs[i].bin_id,move_x,maximal_space.y,temp_bin_group.box_width[i],choose_size);
                move_x += temp_bin_group.box_width[i];
                actual_rects.push_back(tmp_rect);
            }
            result_bins = temp_bin_group.boxs;
        }
        Group current_group = Group(result_bins,virtual_rect,actual_rects);
        return current_group;
    }

    vector<int> freq_sorting(vector<BIN>& bin_list)
    {
        map<int,int> frequency;
        vector<pair<int,int> > vec_frequency;
        vector<int> sorted_frequency;
        BIN tempbin;
        for (size_t i=0;i<bin_list.size();i++)
        {
            tempbin = bin_list[i];
            if (frequency.find(tempbin.bin_length) == frequency.end())
            {
                frequency.insert(make_pair(tempbin.bin_length,0));
            }
            if (frequency.find(tempbin.bin_width) == frequency.end())
            {
                frequency.insert(make_pair(tempbin.bin_width,0));
            }
            frequency[tempbin.bin_length] += 1;
            frequency[tempbin.bin_width] += 1;
        }
        for (map<int,int>::iterator curr=frequency.begin();curr!=frequency.end();curr++)
        {
            vec_frequency.push_back(make_pair(curr->first,curr->second));
        }
        sort(vec_frequency.begin(),vec_frequency.end(),cmp);
        for (size_t i=0;i<vec_frequency.size();i++)
        {
            sorted_frequency.push_back(vec_frequency[i].first);
            // cout << "sorted size: " << vec_frequency[i].first << "frequency: " << vec_frequency[i].second<< endl;
        }
        return sorted_frequency;
    }

    vector<BinCounter> sort_bins_type_by_counter(vector<BIN>& bin_vec)
    {
        vector<BinCounter> size_list;
        map<pair<int,int>,BinCounter> frequency;
        BIN tempbin;
        BinCounter tempcounter;
        for (size_t i=0;i<bin_vec.size();i++)
        {
            tempbin = bin_vec[i];
            if (frequency.find(make_pair(tempbin.bin_length,tempbin.bin_width)) == frequency.end())
            {
                tempcounter = BinCounter(tempbin.bin_length,tempbin.bin_width);
                tempcounter.add_one();
                frequency.insert(make_pair(make_pair(tempbin.bin_length,tempbin.bin_width),tempcounter));
            }
            else
            {
                frequency[make_pair(tempbin.bin_length,tempbin.bin_width)].add_one();
                tempcounter = frequency[make_pair(tempbin.bin_length,tempbin.bin_width)];
                cout << "exist:"<<tempcounter.bin_length<<" "<<tempcounter.bin_width<<" "<<tempcounter.num<<endl;
            }
             
        }
        for (map<pair<int,int>,BinCounter>::iterator curr=frequency.begin();curr!=frequency.end();curr++)
        {
            size_list.push_back(curr->second);
        }
        sort(size_list.begin(),size_list.end(),cmp_counter);
        return size_list;
    }

    vector<RECT> get_bottom_pattern(const vector<pair<int,int> >& cheap_bin_vec,const vector<pair<int,int> >& scarse_bin_vec,int tolerance)
    {
        vector<RECT> rect_vec;
        pair<int,int> bin_pair1;
        pair<int,int> bin_pair2;
        for (size_t i=0;i<cheap_bin_vec.size();i++)
        {
            for (size_t j=0;j<scarse_bin_vec.size();j++)
            {
                bin_pair1 = cheap_bin_vec[i];
                bin_pair2 = scarse_bin_vec[j];
                int pattern1_width = bin_pair1.first + bin_pair1.second + bin_pair2.second;
                int pattern2_width = bin_pair1.second * 2 + bin_pair2.second;
                if (pattern1_width <= width && pattern1_width >= width - tolerance)
                {
                    RECT r1 = RECT("r1",0,0,bin_pair1.second,bin_pair1.first);
                    RECT r2 = RECT("r2",0,0,bin_pair1.first,bin_pair1.second);
                    RECT r3 = RECT("r3",0,0,bin_pair2.second,bin_pair2.first);
                    rect_vec.push_back(r1);
                    rect_vec.push_back(r2);
                    rect_vec.push_back(r3);
                    return rect_vec;
                }
                if (pattern2_width <= width && pattern2_width >= width - tolerance && bin_pair1.first != bin_pair2.first)
                {
                    RECT r1 = RECT("r1",0,0,bin_pair1.second,bin_pair1.first);
                    RECT r2 = RECT("r2",0,0,bin_pair1.second,bin_pair1.first);
                    RECT r3 = RECT("r3",0,0,bin_pair2.second,bin_pair2.first);
                    rect_vec.push_back(r1);
                    rect_vec.push_back(r3);
                    rect_vec.push_back(r2);
                    return rect_vec;
                }
            }
        }
        return rect_vec;
    }

    
    string vehicle_id;
    int width,length,weight;
    float ratio;
    bool allow_flip;
    vector<RECT> used_rects;
    vector<RECT> free_rects;
    vector<BIN> inserted_bins;
    bp::list visited_station;
};

BOOST_PYTHON_MODULE(extract_ext)
{
	using namespace boost::python;
    // bn::initialize();
    
    class_<BIN>("BIN",init<string,int,int,int,string>())
        .def_readonly("bin_id", &BIN::bin_id)
        .def_readonly("bin_weight", &BIN::bin_weight)
        .def_readonly("bin_length", &BIN::bin_length)
        .def_readonly("bin_width", &BIN::bin_width)
        .def_readonly("station", &BIN::station);
    class_<RECT>("RECT", init<string,int,int,int,int>())
        .def("is_contained",&RECT::is_contained)
        .def_readonly("bin_id", &RECT::bin_id)
        .def_readonly("x", &RECT::x)
        .def_readonly("y", &RECT::y)
        .def_readonly("width", &RECT::width)
        .def_readonly("length", &RECT::length);
        
    class_<Vehicle>("Vehicle",init<string,int,int,int,bool>())
        .def_readonly("width", &Vehicle::width)
        .def_readonly("length", &Vehicle::length)
        .def_readwrite("vehicle_id",&Vehicle::vehicle_id)
        .def_readonly("ratio",&Vehicle::ratio)
        .def("update_vehicle",&Vehicle::update_vehicle)
        .def("get_used_rects",&Vehicle::get_used_rects)
        .def("get_inserted_bins",&Vehicle::get_inserted_bins)
        .def("calc_loading_ratio",&Vehicle::calc_loading_ratio)
        .def("set_visited_station",&Vehicle::set_visited_station)
        .def("get_visited_station",&Vehicle::get_visited_station)
        .def("insert_bins_into_vehicle",&Vehicle::insert_bins_into_vehicle)
        .def("insert_big_bins_first",&Vehicle::insert_big_bins_first)
        .def("insert_group_bins_big_first",&Vehicle::insert_group_bins_big_first)
        .def("insert_bins_by_pattern",&Vehicle::insert_bins_by_pattern)
        .def("insert_bins_by_bottom_first",&Vehicle::insert_bins_by_bottom_first)
        .def("insert_group_bins_into_vehicle",&Vehicle::insert_group_bins_into_vehicle);
}

