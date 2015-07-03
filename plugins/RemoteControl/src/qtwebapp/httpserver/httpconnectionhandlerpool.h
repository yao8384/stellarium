#ifndef HTTPCONNECTIONHANDLERPOOL_H
#define HTTPCONNECTIONHANDLERPOOL_H

#include <QList>
#include <QTimer>
#include <QObject>
#include <QMutex>
#include "httpglobal.h"
#include "httpconnectionhandler.h"

/**
 * Contains all settings for the connection handler pool and the child connection handlers.
 */
struct HttpConnectionHandlerPoolSettings : public HttpConnectionHandlerSettings
{
	HttpConnectionHandlerPoolSettings()
		: minThreads(1), maxThreads(100), cleanupInterval(1000)
	{}

	/** The minimal number of threads kept in the pool at all times. Default 1. */
	int minThreads;
	/** The maximal number of threads. If a new connection arrives when this amount of threads is already busy,
	 * the new connection will be dropped. Default 100. */
	int maxThreads;
	/** The time after which inactive threads are stopped in msec. Default 1000.  */
	int cleanupInterval;
};

/**
  Pool of http connection handlers. Connection handlers are created on demand and idle handlers are
  cleaned up in regular time intervals.
  <p>
  Example for the required configuration settings:
  <code><pre>
  minThreads=1
  maxThreads=100
  cleanupInterval=1000
  readTimeout=60000
  ;sslKeyFile=ssl/my.key
  ;sslCertFile=ssl/my.cert
  maxRequestSize=16000
  maxMultiPartSize=1000000
  </pre></code>
  The pool is empty initially and grows with the number of concurrent
  connections. A timer removes one idle connection handler at each
  interval, but it leaves some spare handlers in memory to improve
  performance.
  @see HttpConnectionHandler for description of the ssl settings and readTimeout
  @see HttpRequest for description of config settings maxRequestSize and maxMultiPartSize
*/

class DECLSPEC HttpConnectionHandlerPool : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(HttpConnectionHandlerPool)
public:

    /**
      Constructor.
      @param settings Configuration settings for the HTTP server. Must not be 0.
      @param requestHandler The handler that will process each received HTTP request.
      @warning The requestMapper gets deleted by the destructor of this pool
    */
    HttpConnectionHandlerPool(const HttpConnectionHandlerPoolSettings& settings, HttpRequestHandler* requestHandler);

    /** Destructor */
    virtual ~HttpConnectionHandlerPool();

    /** Get a free connection handler, or 0 if not available. */
    HttpConnectionHandler* getConnectionHandler();

private:

    /** Settings for this pool */
    HttpConnectionHandlerPoolSettings settings;

    /** Will be assigned to each Connectionhandler during their creation */
    HttpRequestHandler* requestHandler;

    /** Pool of connection handlers */
    QList<HttpConnectionHandler*> pool;

    /** Timer to clean-up unused connection handler */
    QTimer cleanupTimer;

    /** Used to synchronize threads */
    QMutex mutex;

private slots:

    /** Received from the clean-up timer.  */
    void cleanup();

};

#endif // HTTPCONNECTIONHANDLERPOOL_H
