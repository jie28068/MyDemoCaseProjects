#include <glog/logging.h>
#include <libpq-fe.h>

class PGSQLSink : public google::LogSink
{
public:
  PGSQLSink()
  {
    conn =
        PQconnectdb("host=localhost user=postgres password=mjc dbname=my_test");
    if (PQstatus(conn) != CONNECTION_OK)
    {
      LOG(ERROR) << "Connection to database failed: " << PQerrorMessage(conn);
      PQfinish(conn);
    }
    // creat table if not exists
    const char *create_table =
        "CREATE TABLE IF NOT EXISTS logs ("
        "id SERIAL PRIMARY KEY,"
        "severity VARCHAR(10) NOT NULL,"
        "filename VARCHAR(255) NOT NULL,"
        "line INT NOT NULL,"
        "time TIMESTAMP NOT NULL,"
        "message TEXT NOT NULL)";

    PGresult *res = PQexec(conn, create_table);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      LOG(ERROR) << "Create table failed: " << PQerrorMessage(conn);
      PQclear(res);
      PQfinish(conn);
    }
  }

  ~PGSQLSink() { PQfinish(conn); }

  void send(google::LogSeverity severity, const char *full_filename,
            const char *base_filename, int line, const struct ::tm *tm_time,
            const char *message, size_t message_len) override
  {
    const char *severity_str;
    switch (severity)
    {
    case google::INFO:
      severity_str = "INFO";
      break;
    case google::WARNING:
      severity_str = "WARNING";
      break;
    case google::ERROR:
      severity_str = "ERROR";
      break;
    case google::FATAL:
      severity_str = "FATAL";
      break;
    default:
      severity_str = "UNKNOWN";
    }

    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_time);

    std::string query =
        "INSERT INTO logs (severity, filename, line, time, message) VALUES ('";
    query += severity_str;
    query += "', '";
    query += base_filename;
    query += "', ";
    query += std::to_string(line);
    query += ", '";
    query += time_str;
    query += "', '";
    query += message;
    query += "')";

    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      LOG(ERROR) << "Insert into database failed: " << PQerrorMessage(conn);
    }
    PQclear(res);
  }

private:
  PGconn *conn;
};

int main()
{
  google::InitGoogleLogging("pgtest");
  FLAGS_minloglevel = google::INFO;
  FLAGS_log_dir = "/home/mjc/log";
  FLAGS_stderrthreshold = google::INFO;
  FLAGS_colorlogtostderr = true;

  PGSQLSink pgsql_sink;
  // google::AddLogSink(&pgsql_sink);

  LOG(INFO) << "This is an INFO log";
  LOG(WARNING) << "This is a WARNING log";
  LOG(ERROR) << "This is an ERROR log";

  // google::RemoveLogSink(&pgsql_sink);

  return 0;
}