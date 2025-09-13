#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<exception>
#include<thread>
#include<boost/asio.hpp>
#include<nlohmann/json.hpp>
#include<mysql/mysql.h>

#define fcc(i, j, k) for(int (i)=(j); (i)<=(k); ++(i))

using boost::asio::ip::tcp;
using nlohmann::json;
using vs = std::vector<std::string>;
using vvs = std::vector<vs>;

constexpr char newl = '\n', snewl[] = " \n";
constexpr int port = 1437;
constexpr char dbip[] = "120.46.180.76";
constexpr char dbuser[] = "myuser";
constexpr char dbpassword[] = "bit123456";
constexpr char dbname[] = "SmartMedical";
constexpr int dbport = 3306;
const vs vs_account{ "username", "type", "reverse" };
const vs vs_patientInfo{ "username", "name", "gender", "birthday", "id", "phoneNumber", "email" };
const vs vs_doctorInfo{ "username", "name", "id", "department", "cost", "begin", "end", "limit" };
const vs vs_namelist{ "username", "name" };
const vs vs_appointment{ "patientUsername", "doctorUsername", "date", "time", "cost", "status" };
const vs vs_case
{ "patientUsername", "doctorUsername", "date", "time", "main", "now", "past", "check", "diagnose" };
const vs vs_advice
{ "patientUsername", "doctorUsername", "date", "time", "medicine", "check", "therapy", "care" };
const vs vs_notice{ "username", "type", "message", "time" };
const vs vs_work{ "username", "date", "status" };
const vs vs_question{ "name", "gender", "age", "height", "weight", "heart", "pressure", "lung" };

template<typename T>
inline void max_(T &t, const T &u) { if(t < u) t = u; }

MYSQL *database = mysql_init(0);
std::set<tcp::socket*> chat_socket;

inline std::string reply_format(std::string s) { return "{\"reply\":\"" + s + "\"}\n"; }
template<typename T>
inline std::string par_format(T t) { return '\'' + std::string(t) + '\''; }
template<typename T>
inline std::string par_format(std::string s, T t) { return '`' + s + "` = " + par_format(t); }
void reply_str(tcp::socket &socket, std::string s)
{
    std::cout << "<-- " << s, std::cout.flush();
    boost::asio::write(socket, boost::asio::buffer(s));
}
void reply_json(tcp::socket &socket, json j) { reply_str(socket, j.dump() + newl); }
int get_json(json &j, json k, std::string s)
{
    if(!k.contains(s)) return 1;
    return j = k[s], 0;
}
json vs_to_json(vs col, vs v)
{
    json ret;
    fcc(i, 1, v.size()) ret[col[i - 1]] = v[i - 1];
    return ret;
}
std::string int_to_str(int i)
{
    std::string ret;
    for(; i; i /= 10) ret.push_back(i % 10 + '0');
    if(ret.empty()) ret += '0';
    return std::reverse(ret.begin(), ret.end()), ret;
}
int str_to_int(std::string s)
{
    int ret = 0;
    for(auto c : s) ret = (ret << 1) + (ret << 3) + (c ^ 48);
    return ret;
}

void print_vvs(vvs v)
{
    int col = v.empty() ? 0 : v[0].size();
    fcc(i, 1, v.size()) fcc(j, 1, col) std::cout << v[i - 1][j - 1] << snewl[j == col];
    std::cout.flush();
}
vvs execute_sql(std::string sql)
{
    std::cout << "<<< " << sql << newl, std::cout.flush();
    vvs ret;
    if(!mysql_query(database, sql.c_str()))
    {
        MYSQL_RES *result = mysql_store_result(database);
        if(result)
        {
            int col = mysql_num_fields(result);
            MYSQL_FIELD *field = mysql_fetch_fields(result);
            std::vector<std::string> vs;
            fcc(i, 0, col - 1) vs.push_back(field[i].name);
            ret.push_back(vs);
            MYSQL_ROW row;
            for(; row = mysql_fetch_row(result);)
            {
                vs.clear();
                fcc(i, 0, col - 1) vs.push_back(row[i] ? row[i] : "NULL");
                ret.push_back(vs);
            }
        }
    }
    std::cout << ">>> " << newl, print_vvs(ret);
    return ret;
}
std::string insert_sql(std::string table, vs col, json j)
{
    std::string sql = "INSERT INTO `" + table + "` VALUES (";
    fcc(i, 1, col.size())
    {
        json k;
        if(get_json(k, j, col[i - 1])) return "no [" + col[i - 1] + ']';
        sql += par_format(k) + ", ";
    }
    sql.pop_back(), sql.pop_back(), sql += ") ON DUPLICATE KEY UPDATE";
    fcc(i, 1, col.size()) sql += " `" + col[i - 1] + "` = VALUES(`" + col[i - 1] + "`),";
    return sql.pop_back(), execute_sql(sql), "successful";
}

void handle_echo(tcp::socket &socket, json j) { reply_json(socket, j); }
void handle_register(tcp::socket &socket, json j)
{
    json username, type, password;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    if(get_json(password, j, "password")) return reply_str(socket, reply_format("no [password]"));
    vvs v = execute_sql(
        "SELECT COUNT(*) FROM `account` WHERE " +
        par_format("username", username) + " AND " +
        par_format("type", type));
    if(v[1][0] != "0") return reply_str(socket, reply_format("failed"));
    std::string reverse(password);
    std::reverse(reverse.begin(), reverse.end()), j["reverse"] = reverse;
    insert_sql("account", vs_account, j);
    json init;
    if(type == "patient")
    {
        init["username"] = username;
        init["name"] = "bao";
        init["gender"] = "male";
        init["birthday"] = "19530615";
        init["id"] = "110108195306151437";
        init["phoneNumber"] = "110";
        init["email"] = "bao@qingfeng.com";
        insert_sql("patientInfo", vs_patientInfo, init);
    }
    if(type == "doctor")
    {
        init["username"] = username;
        init["name"] = "bao";
        init["id"] = "110108195306151437";
        init["department"] = "unknown";
        init["cost"] = "19530615";
        init["begin"] = "0";
        init["end"] = "24";
        init["limit"] = "201307";
        insert_sql("doctorInfo", vs_doctorInfo, init);
    }
    reply_str(socket, reply_format("successful"));
}
void handle_login(tcp::socket &socket, json j)
{
    json username, type, password;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    if(get_json(password, j, "password")) return reply_str(socket, reply_format("no [password]"));
    vvs v = execute_sql(
        "SELECT COUNT(*) FROM `account` WHERE " +
        par_format("username", username) + " AND " +
        par_format("type", type));
    if(v[1][0] == "0") return reply_str(socket, reply_format("usernameWrong"));
    std::string reverse(password);
    std::reverse(reverse.begin(), reverse.end());
    v = execute_sql(
        "SELECT COUNT(*) FROM `account` WHERE " +
        par_format("username", username) + " AND " +
        par_format("type", type) + " AND " +
        par_format("reverse", reverse));
    if(v[1][0] == "0") return reply_str(socket, reply_format("passwordWrong"));
    reply_str(socket, reply_format("successful"));
}
void handle_queryPatientInfo(tcp::socket &socket, json j)
{
    json patientUsername;
    if(get_json(patientUsername, j, "patientUsername"))
        return reply_str(socket, reply_format("no [patientUsername]"));
    vvs v = execute_sql(
        "SELECT * FROM `patientInfo` WHERE " +
        par_format("username", patientUsername));
    json ret;
    ret["data"]["patientInfo"];
    if(v.size() < 2) ret["reply"] = "failed";
    else ret["reply"] = "successful", ret["data"]["patientInfo"] = vs_to_json(vs_patientInfo, v[1]);
    reply_json(socket, ret);
}
void handle_modifyPatientInfo(tcp::socket &socket, json j)
{
    json patientInfo;
    if(get_json(patientInfo, j, "patientInfo"))
        return reply_str(socket, reply_format("no [patientInfo]"));
    reply_str(socket, reply_format(insert_sql("patientInfo", vs_patientInfo, patientInfo)));
}
void handle_queryDoctorInfo(tcp::socket &socket, json j)
{
    json doctorUsername;
    if(get_json(doctorUsername, j, "doctorUsername"))
        return reply_str(socket, reply_format("no [doctorUsername]"));
    vvs v = execute_sql(
        "SELECT * FROM `doctorInfo` WHERE " +
        par_format("username", doctorUsername));
    json ret;
    ret["data"]["doctorInfo"];
    if(v.size() < 2) ret["reply"] = "failed";
    else ret["reply"] = "successful", ret["data"]["doctorInfo"] = vs_to_json(vs_doctorInfo, v[1]);
    reply_json(socket, ret);
}
void handle_modifyDoctorInfo(tcp::socket &socket, json j)
{
    json doctorInfo;
    if(get_json(doctorInfo, j, "doctorInfo"))
        return reply_str(socket, reply_format("no [doctorInfo]"));
    reply_str(socket, reply_format(insert_sql("doctorInfo", vs_doctorInfo, doctorInfo)));
}
void handle_queryPatientList(tcp::socket &socket, json j)
{
    vvs v = execute_sql(
        "SELECT a.username, p.name FROM account a "
        "INNER JOIN patientInfo p ON a.username = p.username "
        "WHERE a.type = \'patient\'");
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
        ret["data"]["patient_" + int_to_str(i)] = vs_to_json(vs_namelist, v[i]);
    reply_json(socket, ret);
}
void handle_queryDoctorList(tcp::socket &socket, json j)
{
    json Time;
    if(get_json(Time, j, "time")) return reply_str(socket, reply_format("no [time]"));
    int t = str_to_int(Time), cou = 0;
    vvs v = execute_sql("SELECT * FROM `doctorInfo`");
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
    {
        json k = vs_to_json(vs_doctorInfo, v[i]);
        int b = str_to_int(k["begin"]), e = str_to_int(k["end"]);
        if(t == 25 || (b <= t && t <= e)) ret["data"]["doctor_" + int_to_str(++cou)] = k;
    }
    reply_json(socket, ret);
}
void handle_queryAppointmentList(tcp::socket &socket, json j)
{
    json username, type;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    vvs v = execute_sql(
        "SELECT * FROM `appointment` WHERE " +
        par_format(std::string(type) + "Username", username));
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
        ret["data"]["appointment_" + int_to_str(i)] = vs_to_json(vs_appointment, v[i]);
    reply_json(socket, ret);
}
void handle_modifyAppointment(tcp::socket &socket, json j)
{
    json appointment;
    if(get_json(appointment, j, "appointment"))
        return reply_str(socket, reply_format("no [appointment]"));
    appointment["cost"] = execute_sql(
        "SELECT `cost` FROM `doctorInfo` WHERE " +
        par_format("username", appointment["doctorUsername"]))[1][0];
    json Case, advice;
    fcc(i, 0, 3) Case[vs_case[i]] = advice[vs_advice[i]] = appointment[vs_appointment[i]];
    fcc(i, 4, vs_case.size() - 1) Case[vs_case[i]] = "unknown";
    fcc(i, 4, vs_advice.size() - 1) advice[vs_advice[i]] = "unknown";
    insert_sql("case", vs_case, Case), insert_sql("advice", vs_advice, advice);
    reply_str(socket, reply_format(insert_sql("appointment", vs_appointment, appointment)));
}
void handle_queryCaseList(tcp::socket &socket, json j)
{
    json username, type;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    vvs v = execute_sql(
        "SELECT * FROM `case` WHERE " +
        par_format(std::string(type) + "Username", username));
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
        ret["data"]["case_" + int_to_str(i)] = vs_to_json(vs_case, v[i]);
    reply_json(socket, ret);
}
void handle_modifyCase(tcp::socket &socket, json j)
{
    json Case;
    if(get_json(Case, j, "case")) return reply_str(socket, reply_format("no [case]"));
    reply_str(socket, reply_format(insert_sql("case", vs_case, Case)));
}
void handle_queryAdviceList(tcp::socket &socket, json j)
{
    json username, type;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    vvs v = execute_sql(
        "SELECT * FROM `advice` WHERE " +
        par_format(std::string(type) + "Username", username));
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
        ret["data"]["advice_" + int_to_str(i)] = vs_to_json(vs_advice, v[i]);
    reply_json(socket, ret);
}
void handle_modifyAdvice(tcp::socket &socket, json j)
{
    json advice;
    if(get_json(advice, j, "advice")) return reply_str(socket, reply_format("no [advice]"));
    reply_str(socket, reply_format(insert_sql("advice", vs_advice, advice)));
}
void handle_queryNoticeList(tcp::socket &socket, json j)
{
    json username, type;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(type, j, "type")) return reply_str(socket, reply_format("no [type]"));
    vvs v = execute_sql(
        "SELECT * FROM `notice` WHERE " +
        par_format("type", "admin") + " OR (" +
        par_format("username", username) + " AND " + par_format("type", type) + ")");
    json ret;
    ret["reply"] = "successful", ret["data"];
    if(!v.empty()) fcc(i, 1, v.size() - 1)
        ret["data"]["notice_" + int_to_str(i)] = vs_to_json(vs_notice, v[i]);
    reply_json(socket, ret);
}
void handle_modifyNotice(tcp::socket &socket, json j)
{
    json notice;
    if(get_json(notice, j, "notice")) return reply_str(socket, reply_format("no [notice]"));
    reply_str(socket, reply_format(insert_sql("notice", vs_notice, notice)));
}
void handle_clock(tcp::socket &socket, json j)
{
    j["status"] = "clock";
    reply_str(socket, reply_format(insert_sql("work", vs_work, j)));
}
void handle_leave(tcp::socket &socket, json j)
{
    j["status"] = "leave";
    reply_str(socket, reply_format(insert_sql("work", vs_work, j)));
}
int judge_question(json q)
{
    int ret = 0;
    int height = str_to_int(q["height"]);
    int weight = str_to_int(q["weight"]);
    int heart = str_to_int(q["heart"]);
    int pressure = str_to_int(q["pressure"]);
    int lung = str_to_int(q["lung"]);
    if(100 <= height && height <= 300) ret |= 1;
    if(30 <= weight && weight <= 130) ret |= 2;
    if(50 <= heart && heart <= 250) ret |= 4;
    if(50 <= pressure && pressure <= 200) ret |= 8;
    if(1000 <= lung && lung <= 9999) ret |= 16;
    return ret;
}
void handle_modifyQuestion(tcp::socket &socket, json j)
{
    json question;
    if(get_json(question, j, "question")) return reply_str(socket, reply_format("no [question]"));
    std::string s = insert_sql("question", vs_question, question);
    json ret;
    ret["reply"] = s, ret["data"]["result"];
    if(s[0] != 'n' || s[1] != 'o')
    {
        int jq = judge_question(question);
        std::string r;
        fcc(i, 0, 4) r += (jq >> i & 1 ? "N" : "Abn")
            + std::string("ormal ") + vs_question[i + 3] + ".;";
        ret["data"]["result"] = r;
    }
    reply_json(socket, ret);
}
constexpr int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
void handle_queryAttendance(tcp::socket &socket, json j)
{
    json username, month;
    if(get_json(username, j, "username")) return reply_str(socket, reply_format("no [username]"));
    if(get_json(month, j, "month")) return reply_str(socket, reply_format("no [month]"));
    int mon = str_to_int(month), clock = 0, leave = 0;
    vvs v = execute_sql("SELECT * FROM `work` WHERE " + par_format("username", username));
    std::map<int, int> mp;
    if(!v.empty()) fcc(i, 1, v.size() - 1)
    {
        json k = vs_to_json(vs_work, v[i]);
        int date = str_to_int(k["date"]);
        if(date / 100 % 100 == mon) max_(mp[date % 100], k["status"] == "clock" ? 2 : 1);
    }
    fcc(i, 1, days[mon])
        if(mp[i] == 2) ++clock;
        else if(mp[i] == 1) ++leave;
    json ret;
    ret["reply"] = "successful";
    ret["data"]["attendance"] =
        "clock " + int_to_str(clock) + " day(s);" +
        "leave " + int_to_str(leave) + " day(s);" +
        "absence " + int_to_str(days[mon] - clock - leave) + " day(s);";
    reply_json(socket, ret);
}
void handle_queryChart(tcp::socket &socket, json j)
{
    int tot = 0, cou[5] = { };
    vvs v = execute_sql("SELECT * FROM `question`");
    if(!v.empty()) fcc(i, 1, v.size() - 1)
    {
        json k = vs_to_json(vs_question, v[i]);
        int jq = judge_question(k);
        ++tot;
        fcc(i, 0, 4) if(!(jq >> i & 1)) ++cou[i];
    }
    std::string s = "Total: " + int_to_str(tot) + ".;";
    fcc(i, 0, 4) s += "Abnormal " + vs_question[i + 3] + ": " + int_to_str(cou[i]) + ".;";
    json ret;
    ret["reply"] = "successful", ret["data"]["chart"] = s;
    reply_json(socket, ret);
}
void handle_chat(tcp::socket &socket, json j)
{
    json ret;
    ret["reply"] = "successful";
    ret["data"]["message"] = '[' + std::string(j["username"]) + "] " + std::string(j["message"]);
    for(auto s : chat_socket) std::cout << "# " << s << newl;
    for(auto s : chat_socket) reply_json(*s, ret);
}
void handle_joinChat(tcp::socket &socket, json j)
{
    std::cout << "+ " << &socket << newl;
    chat_socket.insert(&socket);
    reply_str(socket, reply_format("successful"));
}
void handle_exitChat(tcp::socket &socket, json j)
{
    chat_socket.erase(&socket);
    reply_str(socket, reply_format("successful"));
}
void handle_modifyadminInfoClient(tcp::socket &socket, json j)
{
    if(j.contains("patientInfo")) handle_modifyPatientInfo(socket, j);
    else handle_modifyDoctorInfo(socket, j);
}
int handle(tcp::socket &socket)
{
    char buf[1 << 16];
    boost::system::error_code ec;
    int length = socket.read_some(boost::asio::buffer(buf), ec);
    std::string str;
    fcc(i, 0, length - 1) if(buf[i] != ' ' && buf[i] != '\n') str += buf[i];
    std::cout << "--> " << str << newl, std::cout.flush();
    if(ec) return 1;
    json receive;
    try { receive = json::parse(str); }
    catch(const std::exception &e) { return reply_str(socket, reply_format("jsonError")), 0; }
    json command, data;
    if(get_json(command, receive, "command"))
        return reply_str(socket, reply_format("no [command]")), 0;
    if(get_json(data, receive, "data"))
        return reply_str(socket, reply_format("no [data]")), 0;
    if(command == "echo") handle_echo(socket, receive);
    if(command == "register") handle_register(socket, data);
    if(command == "login") handle_login(socket, data);
    if(command == "queryPatientInfo") handle_queryPatientInfo(socket, data);
    if(command == "modifyPatientInfo") handle_modifyPatientInfo(socket, data);
    if(command == "queryDoctorInfo") handle_queryDoctorInfo(socket, data);
    if(command == "modifyDoctorInfo") handle_modifyDoctorInfo(socket, data);
    if(command == "queryPatientList") handle_queryPatientList(socket, data);
    if(command == "queryDoctorList") handle_queryDoctorList(socket, data);
    if(command == "queryAppointmentList") handle_queryAppointmentList(socket, data);
    if(command == "modifyAppointment") handle_modifyAppointment(socket, data);
    if(command == "queryCaseList") handle_queryCaseList(socket, data);
    if(command == "modifyCase") handle_modifyCase(socket, data);
    if(command == "queryAdviceList") handle_queryAdviceList(socket, data);
    if(command == "modifyAdvice") handle_modifyAdvice(socket, data);
    if(command == "queryNoticeList") handle_queryNoticeList(socket, data);
    if(command == "modifyNotice") handle_modifyNotice(socket, data);
    if(command == "clock") handle_clock(socket, data);
    if(command == "leave") handle_leave(socket, data);
    if(command == "modifyQuestion") handle_modifyQuestion(socket, data);
    if(command == "queryAttendance") handle_queryAttendance(socket, data);
    if(command == "queryChart") handle_queryChart(socket, data);
    if(command == "chat") handle_chat(socket, data);
    if(command == "joinChat") handle_joinChat(socket, data);
    if(command == "exitChat") handle_exitChat(socket, data);
    if(command == "modifyadminInfoClient") handle_modifyadminInfoClient(socket, data);
    return 0;
}
void handle_client(tcp::socket *socket)
{
    std::string ip = "unknown";
    try { ip = socket->remote_endpoint().address().to_string(); }
    catch(const std::exception &e) { }
    std::cout << '[' << ip << ']' << " Client connected" << newl;
    reply_str(*socket, reply_format("successful_connection"));
    for(; !handle(*socket););
    std::cout << '[' << ip << ']' << " Client disconnected" << newl;
    handle_exitChat(*socket, { });
    delete socket;
}
int main()
{
    // mysql -h 120.46.180.76 -P 3306 -u myuser -p SmartMedical
    if(!mysql_real_connect(database, dbip, dbuser, dbpassword, dbname, dbport, 0, 0))
    {
        std::cout << "Database connection failed: " << mysql_error(database) << newl;
        return mysql_close(database), 0;
    }
    std::cout << "Database connection successful" << newl;
    boost::asio::io_service service;
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), port));
    std::cout << "HospitalServer is listening on port " << port << newl;
    for(;;)
    {
        tcp::socket *socket = new tcp::socket(service);
        acceptor.accept(*socket);
        std::thread(handle_client, socket).detach();
    }
}