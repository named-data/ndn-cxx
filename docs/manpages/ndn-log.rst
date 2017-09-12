ndn-log
=======

The ndn-cxx logging facility exposes the internal state of NDN libraries and
applications so the user can investigate internal interactions, such as interest
dispatch inside ndn::Face, Data and Interest validation in the security framework,
sync and recovery Interest processing inside ChronoSync, etc. During runtime, the
user is able to specify the types of messages he or she would like to receive from
a set of logging modules pre-configured by library and application developers.

Environment Variable
--------------------

One way to control ndn-cxx logging facility is through the environment variable
``NDN_LOG``. Using this variable, one can set the log levels for the available logging
modules. Logging modules within different libraries and applications usually have
distinguishing prefixes (``ndn.``, ``sync.``, etc.), which can be specified when
setting the environment variable. Wildcards can be used to enable logging for all
modules (just ``*``) or all modules under a selected prefix (e.g., ``ndn.*``).

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

    export NDN_LOG="<prefix1>=<loglevel1>:<prefix2>=<loglevel2>"

**Examples:**

::

    export NDN_LOG="ndn.*=DEBUG"
    export NDN_LOG="ndn.UnixTransport=INFO"
    export NDN_LOG="sync.Logic=ERROR"
    export NDN_LOG="*=DEBUG:ndn.UnixTransport=INFO:sync.Logic=ERROR"

**Note:**

Shorter (general) prefixes should be placed before longer (specific) prefixes.
Otherwise, the specific prefix's loglevel will be overwritten. For example,
`export NDN_LOG="ndn.UnixTransport=TRACE:ndn.*=ERROR:*=INFO"` sets all modules
to INFO; it should be written as
`export NDN_LOG="*=INFO:ndn.*=ERROR:ndn.UnixTransport=TRACE"` for the desired effect.

**Note:**

Setting the environment variable with sudo requires the application to be run
in the same command.

::

    Correct:

        sudo env NDN_LOG=logLevel ./ndn-application

        sudo -s
        export NDN_LOG=logLevel
        ./ndn-application

    Incorrect:

        sudo export NDN_LOG=logLevel
        sudo ./ndn-application

        NDN_LOG=logLevel sudo ./ndn-application
