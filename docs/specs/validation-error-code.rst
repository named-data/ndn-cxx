Validation Error Code
=====================

The following table defines a list of known codes and their description, which can be returned from the :ndn-cxx:`v2::Validator` interface.
Other error codes can be returned by validator implementations outside ndn-cxx codebase.

+------------+--------------------------+-----------------------------------------------------+
| Error code | Short ID                 | Description                                         |
+============+==========================+=====================================================+
| 0          | NO_ERROR                 | No error                                            |
+------------+--------------------------+-----------------------------------------------------+
| 1          | INVALID_SIGNATURE        | Invalid signature                                   |
+------------+--------------------------+-----------------------------------------------------+
| 2          | NO_SIGNATURE             | Missing signature                                   |
+------------+--------------------------+-----------------------------------------------------+
| 3          | CANNOT_RETRIEVE_CERT     | Cannot retrieve certificate                         |
+------------+--------------------------+-----------------------------------------------------+
| 4          | EXPIRED_CERT             | Certificate expired                                 |
+------------+--------------------------+-----------------------------------------------------+
| 5          | LOOP_DETECTED            | Loop detected in certification chain                |
+------------+--------------------------+-----------------------------------------------------+
| 6          | MALFORMED_CERT           | Malformed certificate                               |
+------------+--------------------------+-----------------------------------------------------+
| 7          | EXCEEDED_DEPTH_LIMIT     | Exceeded validation depth limit                     |
+------------+--------------------------+-----------------------------------------------------+
| 8          | INVALID_KEY_LOCATOR      | Key locator violates validation policy              |
+------------+--------------------------+-----------------------------------------------------+
| ..         | ...                      | ...                                                 |
+------------+--------------------------+-----------------------------------------------------+
| 255        | IMPLEMENTATION_ERROR     | Internal implementation error                       |
+------------+--------------------------+-----------------------------------------------------+

Specialized validator implementations can use error codes >= 256 to indicate a specialized error.
