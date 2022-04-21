ndn-cxx logging
===============

Description
-----------

The ndn-cxx logging facility exposes the internal state of NDN libraries and
applications so the user can investigate internal interactions, such as interest
dispatch inside ndn::Face, Data and Interest validation in the security framework,
sync and recovery Interest processing inside ChronoSync, etc. During runtime, the
user is able to specify the types of messages he or she would like to receive from
a set of logging modules pre-configured by library and application developers.

Environment
-----------

One way to control ndn-cxx logging facility is through the environment variable
``NDN_LOG``. Using this variable, one can set the log levels for the available logging
modules. Logging modules within different libraries and applications usually have
distinguishing prefixes (``ndn.``, ``sync.``, etc.), which can be specified when
setting the environment variable. Wildcards can be used to enable logging for all
modules (just ``*``) or all modules under a selected prefix (e.g., ``ndn.*``).

If an additional environment variable ``NDN_LOG_NOFLUSH`` is set, the automatic flushing
after each log record will be disabled. This can improve logging performance but may
cause the log records to appear delayed or, in case of application crash, the last
few log records may be lost.

ndn-cxx logging facility provides a mechanism to manage the type of log messages
that are written by classifying log messages by severity levels. Listed below
are the available log levels.

**Log Levels:**

::

    TRACE
    DEBUG
    INFO
    WARN
    ERROR
    FATAL

A message's severity level will determine whether the log is written. For instance,
if an application sets its log severity level to DEBUG, all messages marked with
DEBUG, or any of those below that level, are written. FATAL level logs are always
written.

Setting NDN_LOG requires the following syntax with as many prefixes and
corresponding loglevels as the user desires:

.. code-block:: sh

    export NDN_LOG="<prefix1>=<loglevel1>:<prefix2>=<loglevel2>"

*Example:*

.. code-block:: sh

    export NDN_LOG="ndn.*=DEBUG"
    export NDN_LOG="ndn.UnixTransport=INFO"
    export NDN_LOG="sync.Logic=ERROR"
    export NDN_LOG="*=DEBUG:ndn.UnixTransport=INFO:sync.Logic=ERROR"

**Note:**

The loglevel assignments in ``NDN_LOG`` are processed left-to-right. Thus, shorter
(more general) prefixes should be listed before longer (more specific) prefixes.
Otherwise, the loglevel setting of a more specific prefix may be overwritten by a
more general assignment appearing later in the string. For example:

.. code-block:: sh

    export NDN_LOG="ndn.UnixTransport=TRACE:ndn.*=ERROR:*=INFO"

will set all modules to INFO. To obtain the desired effect, it should instead be
written as:

.. code-block:: sh

    export NDN_LOG="*=INFO:ndn.*=ERROR:ndn.UnixTransport=TRACE"

**Note:**

Setting the environment variable with sudo requires the application to be run
in the same command.

.. code-block:: sh

    # Correct
    sudo env NDN_LOG=logLevel ./ndn-application

    # Also correct
    sudo -s
    export NDN_LOG=logLevel
    ./ndn-application

    # Incorrect
    sudo export NDN_LOG=logLevel
    sudo ./ndn-application

    # Incorrect
    NDN_LOG=logLevel sudo ./ndn-application
