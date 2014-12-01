In test, we set a test-specific "HOME", which cause OS X keychain look for the
default keychain of a "different" user. If the default keychain does not exist,
all subsequent calls to OS X keychain will fail. User interaction (such as
specifying password) is required to create a keychain. However, user interaction
is not feasible in automated tests.

This problem is caused by the OS X system assumption that one user must have a
login keychain, which is also the user's default keychain, because a user
account is always created with a login keychain as default. Thus OS X system
infers a user according to the HOME env, and did not expect user to change the
HOME env in normal use.
