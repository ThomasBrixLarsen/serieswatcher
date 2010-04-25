#ifndef PARSE_THREAD_H
# define PARSE_THREAD_H

class ParseThread : public QThread
{
public:
  ParseThread();
  ~ParseThread();

  void parse(const QByteArray & data);

signals:
  void progress(qint64 done, qint64 total);

private:
  QQueue < QByteArray > jobs;
};

#endif
