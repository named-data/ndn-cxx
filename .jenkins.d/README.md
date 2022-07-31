# Continuous Integration Scripts

## Environment Variables

- `NODE_LABELS`: space-separated list of platform properties. The included values are used by
  the build scripts to select the proper behavior for different operating systems and versions.

  The list should normally contain `[OS_TYPE]`, `[DISTRO_TYPE]`, and `[DISTRO_VERSION]`.

  Example values:

  - `[OS_TYPE]`: `Linux`, `macos`
  - `[DISTRO_TYPE]`: `Ubuntu`, `CentOS`
  - `[DISTRO_VERSION]`: `ubuntu-20.04`, `ubuntu-22.04`, `centos-9`, `macos-10.15`, `macos-11`

- `JOB_NAME`: optional variable that defines the type of build job. Depending on the job type,
  the build scripts can perform different tasks.

  Possible values:

  - empty: default build task
  - `code-coverage`: debug build with tests and code coverage analysis (Ubuntu Linux is assumed)
  - `limited-build`: only a single debug build with tests

- `CACHE_DIR`: directory containing cached files from previous builds, e.g., a compiled version
  of ndn-cxx. If not set, `/tmp` is used.

- `WAF_JOBS`: number of parallel build threads used by waf, defaults to 1.
