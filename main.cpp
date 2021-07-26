// windows 11 theme changer

#include<bits/stdc++.h>
#include<math.h>
#include<unistd.h>
#include<windows.h>

struct Date{
    int day, month, year;
    Date(int _day, int _month, int _year) :
    day(_day), month(_month), year(_year){}
};

Date today(){
    time_t now = time(0);
    tm t = *localtime(&now);
    return {t.tm_mday, t.tm_mon + 1, t.tm_year + 1900};
}

inline bool is_leap(int year){
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

inline bool is_leap(Date date){
    return is_leap(date.year);
}

int day_of_year(Date date){
    int days_table[2][12] = {
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
    };

    return days_table[is_leap(date.year)][date.month - 1] + date.day;
}

struct Time{
    int hr, mn;
    Time(){}
    Time(int _hr, int _mn) :
    hr(_hr), mn(_mn){}
};

Time now_utc(){
    time_t now = time(0);
    tm time_utc = *gmtime(&now);
    return {time_utc.tm_hour, time_utc.tm_min};
}

bool operator>=(Time t1, Time t2){
    return t1.hr > t2.hr || (t1.hr == t2.hr && t1.mn >= t2.mn);
}

bool operator<=(Time t1, Time t2){
    return t1.hr < t2.hr || (t1.hr == t2.hr && t1.mn <= t2.mn);
}

typedef int Timezone;

Timezone curr_timezone(){
    time_t now = time(0);
    tm time_local = *localtime(&now);
    tm time_utc = *gmtime(&now);
    return time_local.tm_hour - time_utc.tm_hour;
}

struct Coord{
    double lat, lon;
    Coord(){}
    Coord(double _lat, double _lon) :
    lat(_lat), lon(_lon){}
};

inline double to_rad(double angle){ return angle * 0.0174533; }
inline double to_deg(double angle){ return angle * 57.2958; }

inline double cosd(double angle){ return cos(to_rad(angle)); }
inline double tand(double angle){ return tan(to_rad(angle)); }

class SunData{
public:
    SunData(Coord _coord, Timezone _tz = 0) :
    m_coord(_coord), m_date(today()), m_tz(_tz) { m_init(); }

    Time get_noon() const { return {m_noon.hr + m_tz, m_noon.mn}; }
    Time get_sunrise() const { return {m_sunrise.hr + m_tz, m_sunrise.mn}; }
    Time get_sunset() const { return {m_sunset.hr + m_tz, m_sunset.mn}; }

    Time get_noon_utc() const { return m_noon; }
    Time get_sunrise_utc() const { return m_sunrise; }
    Time get_sunset_utc() const { return m_sunset; }
private:
    Date m_date;                // 
    Coord m_coord;              // 

    double m_fract;             // fractional year
    double m_eqtime;            // equation of time
    double m_decl;              // declination angle
    double m_ha;                // hour angle

    Time m_noon;                // solar noon (UTC)
    Time m_sunrise;             // sunrise time (UTC)
    Time m_sunset;              // sunset time (UTC)

    Timezone m_tz;              // 

    void m_init();              // calculate noon, sunset and sunrise
    void m_get_fract();         // 
    void m_get_eqtime();        // 
    void m_get_decl();          // 
    void m_get_hour_angle();    // 
    void m_get_noon();          // 
    void m_get_sunrise();       // 
    void m_get_sunset();        // 
};

inline void SunData::m_get_fract(){
    m_fract = 2 * M_PI / (is_leap(m_date.year) ? 366. : 365.) * (day_of_year(m_date) - 1);
}

void SunData::m_get_eqtime(){
    double g = m_fract;
    m_eqtime = 229.18 * (0.000075 + 0.001868*cos(g) - 0.032077*sin(g) - 0.014615*cos(2*g) - 0.040849*sin(2*g));
}

void SunData::m_get_decl(){
    double g = m_fract;
    m_decl = 0.006918 - 0.399912*cos(g) + 0.070257*sin(g) - 0.006758*cos(2*g) + 0.000907*sin(2*g) - 0.002697*cos(3*g) + 0.00148*sin(3*g);
}

inline void SunData::m_get_hour_angle(){
    m_ha = acos((cosd(90.833) / (cosd(m_coord.lat) * cos(m_decl))) - tand(m_coord.lat) * tan(m_decl));
}

void SunData::m_get_noon(){
    m_noon = {0, 0};
}

void SunData::m_get_sunrise(){
    int mins = 720 - 4 * (m_coord.lon + to_deg(m_ha)) - m_eqtime;
    m_sunrise = {mins / 60, mins % 60};
}

inline void SunData::m_get_sunset(){
    int mins = 720 - 4 * (m_coord.lon + to_deg(-m_ha)) - m_eqtime;
    m_sunset = {mins / 60, mins % 60};
}

void SunData::m_init(){
    m_get_fract();
    m_get_eqtime();
    m_get_decl();
    m_get_hour_angle();
    m_get_noon();
    m_get_sunrise();
    m_get_sunset();
}

enum State{
    dark,
    light
};

void change_theme(State theme){
    printf("CHANGE THEME | %02d:%02d\n", now_utc().hr, now_utc().mn);
    if(theme == light){
        printf("LIGHT\n");
        system("powershell New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name SystemUsesLightTheme -Value 1 -Type Dword -Force; New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name AppsUseLightTheme -Value 1 -Type Dword -Force");
    }
    if(theme == dark){
        printf("DARK");
        system("powershell New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name SystemUsesLightTheme -Value 0 -Type Dword -Force; New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize -Name AppsUseLightTheme -Value 0 -Type Dword -Force");
    }
}

Coord read_config(){
    Coord coord;
    std::string config_path;
    getcwd(&config_path[0], 512);
    config_path += "tc_config.cfg";
    std::ifstream f_in(config_path, std::ios::in);
    f_in >> coord.lat >> coord.lon;
    f_in.close();

    std::cout << coord.lat << " " << coord.lon << std::endl;
    return coord;
}

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

int main(){
    HideConsole();

    Coord here = read_config();
    SunData data(here);

    Time sunrise = data.get_sunrise_utc();
    Time sunset = data.get_sunset_utc();

    State curr_state = (now_utc() >= sunrise && now_utc() <= sunset) ? light : dark;
    change_theme(curr_state);

    while(true){
        State tmp = (now_utc() >= sunrise && now_utc() <= sunset) ? light : dark;
        if(tmp != curr_state){
            curr_state = tmp;
            change_theme(curr_state);
        }

        Sleep(120000);
    }

    return 0;
}
