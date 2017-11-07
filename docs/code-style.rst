ndn-cxx Code Style and Coding Guidelines
========================================

Based on

    * "C++ Programming Style Guidelines" by Geotechnical Software Services, Copyright © 1996 – 2011.
      The original document is available at `<http://geosoft.no/development/cppstyle.html>`_

    * NDN Platform "C++, C, C#, Java and JavaScript Code Guidelines".
      The original document available at `<http://named-data.net/codebase/platform/documentation/ndn-platform-development-guidelines/cpp-code-guidelines/>`_

1. Code layout
--------------

1.1. The code layout should generally follow the GNU coding standard layout for C,
extended it to C++.

    * Do not use tabs for indentation.
    * Indentation spacing is 2 spaces.
    * Lines should be within a reasonable range.  Lines longer than 100 columns should
      generally be avoided.

1.2. Whitespace

    * Conventional operators (``if``, ``for``, ``while``, and others) should be
      surrounded by a space character.
    * Commas should be followed by a white space.
    * Semicolons in for statments should be followed by a space character.

    Examples:

    .. code-block:: c++

        a = (b + c) * d; // NOT: a=(b+c)*d

        while (true) {     // NOT: while(true)
          ...

        doSomething(a, b, c, d);   // NOT: doSomething(a,b,c,d);

        for (i = 0; i < 10; i++) { // NOT: for(i=0;i<10;i++){
          ...

1.3. Namespaces should have the following form:

    .. code-block:: c++

        namespace example {

        code;
        moreCode;

        } // namespace example

    Note that code inside namespace is **not** indented. Avoid the following:

    .. code-block:: c++

        // NOT
        //
        // namespace example {
        //
        //   code;
        //   moreCode;
        //
        // } // namespace example

1.4. Class declarations should have the following form:

    .. code-block:: c++

        class SomeClass : public BaseClass
        {
        public:
          ... <public methods> ...

        protected:
          ... <protected methods> ...

        private:
          ... <private methods> ...

        public:
          ... <public data> ...

        protected:
          ... <protected data> ...

        private:
          ... <private data> ...
        };

    ``public``, ``protected``, ``private`` may be repeated several times without
    interleaving (e.g., public, public, public, private, private) if this improves
    readability of the code.

    Nested classes can be defined in appropriate visibility section, either in methods
    block, data block, or in a separate section (depending which one provides better code
    readability).

1.5. Method and function definitions should have the following form:

    .. code-block:: c++

        void
        someMethod()
        {
          ...
        }

        void
        SomeClass::someMethod()
        {
          ...
        }

1.6. The ``if-else`` class of statements should have the following form:

    .. code-block:: c++

        if (condition) {
          statements;
        }

        if (condition) {
          statements;
        }
        else {
          statements;
        }

        if (condition) {
          statements;
        }
        else if (condition) {
          statements;
        }
        else {
          statements;
        }

1.7. A ``for`` statement should have the following form:

    .. code-block:: c++

        for (initialization; condition; update) {
          statements;
        }

    An empty ``for`` statement should have the following form:

    .. code-block:: c++

        for (initialization; condition; update)
          ;

    This emphasizes the fact that the ``for`` statement is empty and makes it obvious for
    the reader that this is intentional.  Empty loops should be avoided however.

1.8. A ``while`` statement should have the following form:

    .. code-block:: c++

        while (condition) {
          statements;
        }

1.9. A ``do-while`` statement should have the following form:

    .. code-block:: c++

        do {
          statements;
        } while (condition);

1.10. A ``switch`` statement should have the following form:

    .. code-block:: c++

        switch (condition) {
          case ABC:        // 2 space indent
            statements;    // 4 space indent
            NDN_CXX_FALLTHROUGH;

          case DEF:
            statements;
            break;

          case XYZ: {
            statements;
            break;
          }

          default:
            statements;
            break;
        }

    When curly braces are used inside a ``case`` block, the braces must cover the entire
    ``case`` block.

    .. code-block:: c++

        switch (condition) {
          // Correct style
          case A0: {
            statements;
            break;
          }

          // Correct style
          case A1: {
            statements;
            NDN_CXX_FALLTHROUGH;
          }

          // Incorrect style: braces should cover the entire case block
          case B: {
            statements;
          }
          statements;
          break;

          default:
            break;
        }

    The following style is still allowed when none of the ``case`` blocks has curly braces.

    .. code-block:: c++

        switch (condition) {
        case ABC:        // no indent
          statements;    // 2 space indent
          NDN_CXX_FALLTHROUGH;

        case DEF:
          statements;
          break;

        default:
          statements;
          break;
        }

    The ``NDN_CXX_FALLTHROUGH;`` annotation must be included whenever there is
    a case without a break statement. Leaving the break out is a common error,
    and it must be made clear that it is intentional when it is not there.
    Moreover, modern compilers will warn when a case that falls through is not
    explicitly annotated.

1.11. A ``try-catch`` statement should have the following form:

    .. code-block:: c++

        try {
          statements;
        }
        catch (const Exception& exception) {
          statements;
        }

1.12. The incompleteness of split lines must be made obvious.

    .. code-block:: c++

        totalSum = a + b + c +
                   d + e;
        function(param1, param2,
                 param3);
        for (int tableNo = 0; tableNo < nTables;
             tableNo += tableStep) {
          ...
        }

    Split lines occur when a statement exceeds the column limit given in rule 1.1. It is
    difficult to give rigid rules for how lines should be split, but the examples above should
    give a general hint. In general:

    * Break after a comma.
    * Break after an operator.
    * Align the new line with the beginning of the expression on the previous line.

    Exceptions:

    * The following is standard practice with ``operator<<``:

        .. code-block:: c++

            std::cout << "Something here "
                      << "Something there" << std::endl;

1.13.  When class variables need to be initialized in the constructor, the initialization
should take the following form:

    .. code-block:: c++

        SomeClass::SomeClass(int value, const std::string& string)
          : m_value(value)
          , m_string(string)
          ...
        {
        }

    Each initialization should be put on a separate line, starting either with the colon
    for the first initialization or with comma for all subsequent initializations.

1.14. A range-based ``for`` statement should have the following form:

    .. code-block:: c++

        for (T i : range) {
          statements;
        }

1.15. A lambda expression should have the following form:

    .. code-block:: c++

        [&capture1, capture2] (T1 arg1, T2 arg2) {
          statements;
        }

        [&capture1, capture2] (T1 arg1, T2 arg2) mutable {
          statements;
        }

        [this] (T arg) {
          statements;
        }

    If the lambda has no parameters, ``()`` should be omitted.

    .. code-block:: c++

        [&capture1, capture2] {
          statements;
        }

    Either capture-default (``[&]`` or ``[=]``) is permitted, but its usage should be minimized.
    Only use a capture-default when it significantly simplifies code and improves readability.

    .. code-block:: c++

        [&] (T arg) {
          statements;
        }

        [=] (T arg) {
          statements;
        }

    Trailing return type should be omitted whenever possible. Add it only when the compiler
    cannot deduce the return type automatically, or when it improves readability.
    ``()`` is required by the C++ standard when ``mutable`` or a trailing return type is used.

    .. code-block:: c++

        [] (T arg) -> int {
          statements;
        }

        [] () -> int {
          statements;
        }

    If the function body has only one line, and the whole lambda expression can fit in one line,
    the following form is also acceptable:

    .. code-block:: c++

        [&capture1, capture2] (T1 arg1, T2 arg2) { statement; }

    No-op can be written in a more compact form:

    .. code-block:: c++

        []{}

1.16. List initialization should have the following form:

    .. code-block:: c++

        T object{arg1, arg2};

        T{arg1, arg2};

        new T{arg1, arg2};

        return {arg1, arg2};

        function({arg1, arg2}, otherArgument);

        object[{arg1, arg2}];

        T({arg1, arg2})

        class Class
        {
        private:
          T m_member = {arg1, arg2};
          static T s_member = {arg1, arg2};
        };

        Class::Class()
          : m_member{arg1, arg2}
        {
        }

        T object = {arg1, arg2};

    An empty braced-init-list is written as ``{}``. For example:

    .. code-block:: c++

        T object{};

        T object = {};

2. Naming Conventions
---------------------

2.1. C++ header files should have the extension ``.hpp``. Source files should have the
extension ``.cpp``

    File names should be all lower case. If the class name
    is a composite of several words, each word in a file name should be separated with a
    dash (-). A class should be declared in a header file and defined in a source file
    where the name of the files match the name of the class.

    ::

        my-class.hpp, my-class.cpp


2.2. Names representing types must be written in English in mixed case starting with upper case.

    .. code-block:: c++

        class MyClass;
        class Line;
        class SavingsAccount;

2.3. Variable names must be written in English in mixed case starting with lower case.

    .. code-block:: c++

        MyClass myClass;
        Line line;
        SavingsAccount savingsAccount;
        int theAnswerToLifeTheUniverseAndEverything;

2.4. Named constants (including enumeration values) must be all uppercase using underscore
to separate words.

    .. code-block:: c++

        const int MAX_ITERATIONS = 25;
        const std::string COLOR_RED = "red";
        static const double PI = 3.14;

    In some cases, it is a better (or is the only way for complex constants in header-only
    classes) to implement the value as a method.

    .. code-block:: c++

        static int          // declare constexpr if possible
        getMaxIterations()
        {
          return 25;
        }

2.5. Names representing methods or functions must be commands starting with a verb and
written in mixed case starting with lower case.

    .. code-block:: c++

        std::string
        getName()
        {
          ...
        }

        double
        computeTotalWidth()
        {
          ...
        }

2.6. Names representing namespaces should be all lowercase.

    .. code-block:: c++

        namespace model {
        namespace analyzer {

        ...

        } // namespace analyzer
        } // namespace model

2.7. Names representing generic template types should be a single uppercase letter.

    .. code-block:: c++

        template<class T> ...
        template<class C, class D> ...

    However, when a template parameter represents a certain concept and is expected
    to have a certain interface, the name should be explicitly spelled out.

    .. code-block:: c++

        template<class InputIterator> ...
        template<class Packet> ...

2.8. Abbreviations and acronyms must not be uppercase when used as name.

    .. code-block:: c++

        exportHtmlSource(); // NOT: exportHTMLSource();
        openDvdPlayer();    // NOT: openDVDPlayer();

2.9. Global variables should have ``g_`` prefix

    .. code-block:: c++

        g_mainWindow.open();
        g_applicationContext.getName();

    In general, the use of global variables should be avoided.  Consider using singleton
    objects instead.

2.10. Private class variables should have ``m_`` prefix. Static class variables should have
``s_`` prefix.

    .. code-block:: c++

        class SomeClass
        {
        private:
          int m_length;

          static std::string s_name;
        };


2.11. Variables with a large scope should have long (explicit) names, variables with a small
scope can have short names.

    Scratch variables used for temporary storage or indices are best kept short.  A
    programmer reading such variables should be able to assume that its value is not used
    outside of a few lines of code. Common scratch variables for integers are ``i``,
    ``j``, ``k``, ``m``, ``n`` and for characters ``c`` and ``d``.

2.12. The name of the object is implicit, and should be avoided in a method name.

    .. code-block:: c++

        line.getLength(); // NOT: line.getLineLength();

    The latter seems natural in the class declaration, but proves superfluous in use, as
    shown in the example.

2.13. The terms ``get/set`` must be used where an attribute is accessed directly.

    .. code-block:: c++

        employee.getName();
        employee.setName(name);

        matrix.getElement(2, 4);
        matrix.setElement(2, 4, value);

2.14. The term ``compute`` can be used in methods where something is computed.

    .. code-block:: c++

        valueSet.computeAverage();
        matrix.computeInverse()

    Give the reader the immediate clue that this is a potentially time-consuming operation,
    and if used repeatedly, he might consider caching the result. Consistent use of the term
    enhances readability.

2.15. The term ``find`` can be used in methods where something is looked up.

    .. code-block:: c++

        vertex.findNearestVertex();
        matrix.findMinElement();

    Give the reader the immediate clue that this is a simple look up method with a minimum
    of computations involved. Consistent use of the term enhances readability.

2.16. Plural form should be used on names representing a collection of objects.

    .. code-block:: c++

        vector<Point> points;
        int values[];

    Enhances readability since the name gives the user an immediate clue of the type of
    the variable and the operations that can be performed on its elements.

2.17. The prefix ``n`` should be used for variables representing a number of objects.

    .. code-block:: c++

        nPoints, nLines

    The notation is taken from mathematics where it is an established convention for
    indicating a number of objects.


2.18. The suffix ``No`` should be used for variables representing an entity number.

    .. code-block:: c++

        tableNo, employeeNo

    The notation is taken from mathematics where it is an established convention for
    indicating an entity number.  An elegant alternative is to prefix such variables with
    an ``i``: ``iTable``, ``iEmployee``.  This effectively makes them named iterators.

2.19. The prefix ``is``, ``has``, ``need``, or similar should be used for boolean variables and
methods.

    .. code-block:: c++

        isSet, isVisible, isFinished, isFound, isOpen
        needToConvert, needToFinish

2.20. Complement names must be used for complement operations, reducing complexity by
symmetry.

    ::

        get/set, add/remove, create/destroy, start/stop, insert/delete,
        increment/decrement, old/new, begin/end, first/last, up/down, min/max,
        next/previous (and commonly used next/prev), open/close, show/hide,
        suspend/resume, etc.

    Pair ``insert/erase`` should be preferred.  ``insert/delete`` can also be used if it
    does not conflict with C++ delete keyword.

2.21. Variable names should not include reference to variable type (do not use Hungarian
notation).

    .. code-block:: c++

        Line* line; // NOT: Line* pLine;
                    // NOT: Line* linePtr;

        size_t nPoints; // NOT lnPoints

        char* name; // NOT szName

2.22. Negated boolean variable names should be avoided.

    .. code-block:: c++

        bool isError; // NOT: isNoError
        bool isFound; // NOT: isNotFound

2.23. Enumeration constants recommended to prefix with a common type name.

    .. code-block:: c++

        enum Color {
          COLOR_RED,
          COLOR_GREEN,
          COLOR_BLUE
        };

2.24. Exceptions can be suffixed with either ``Exception`` (e.g., ``SecurityException``) or
``Error`` (e.g., ``SecurityError``).

    The recommended method is to declare exception class ``Exception`` or ``Error`` as an
    inner class, from which the exception is thrown.  For example, when declaring class
    ``Foo`` that can throw errors, one can write the following:

    .. code-block:: c++

        #include <stdexcept>

        class Foo
        {
          class Error : public std::runtime_error
          {
          public:
            explicit
            Error(const std::string& what)
              : std::runtime_error(what)
            {
            }
          };
        };

    In addition to that, if class Foo is a base class or interface for some class
    hierarchy, then child classes should should define their own ``Error`` or
    ``Exception`` classes that are inherited from the parent's Error class.


2.25. Functions (methods returning something) should be named after what they return and
procedures (void methods) after what they do.

    Increase readability. Makes it clear what the unit should do and especially all the
    things it is not supposed to do. This again makes it easier to keep the code clean of
    side effects.

3. Miscellaneous
----------------

3.1. Exceptions can be used in the code, but should be used only in exceptional cases and
not in the primary processing path.

3.2. Header files must contain an include guard.

    For example, header file located in ``module/class-name.hpp`` or in
    ``src/module/class-name.hpp`` should have header guard in the following form:

    .. code-block:: c++

        #ifndef APP_MODULE_CLASS_NAME_HPP
        #define APP_MODULE_CLASS_NAME_HPP
        ...
        #endif // APP_MODULE_CLASS_NAME_HPP

    The name should follow the location of the file inside the source tree and prevents
    naming conflicts.  Header guard should be prefixed with the application/library name
    to avoid conflicts with other packaged and libraries.

3.3. Header files which are in the same source distribution should be included in
``"quotes"``, if possible with a path relative to the source file.  Header files for
system and other external libraries should be included in ``<angle brackets>``.

    .. code-block:: c++

        #include <string>
        #include <boost/lexical_cast.hpp>

        #include "util/random.hpp"

3.4. Include statements should be grouped. Same-project headers should be included first.
Leave an empty line between groups of include statements. Sort alphabetically within a group.

    .. code-block:: c++

        #include "detail/pending-interest.hpp"
        #include "util/random.hpp"

        #include <fstream>
        #include <iomanip>

        #include <boost/lexical_cast.hpp>
        #include <boost/regex.hpp>


3.5. Types that are local to one file only can be declared inside that file.


3.6. Implicit conversion is generally allowed.

    Implicit conversion between integer and floating point numbers can cause problems and
    should be avoided.

    Implicit conversion in single-argument constructor is usually undesirable. Therefore, all
    single-argument constructors should be marked 'explicit', unless implicit conversion is
    desirable. In that case, a comment should document the reason.

    Avoid C-style casts. Use ``static_cast``, ``dynamic_cast``, ``reinterpret_cast``,
    ``const_cast`` instead where appropriate.  Use ``static_pointer_cast``,
    ``dynamic_pointer_cast``, ``const_pointer_cast`` when dealing with ``shared_ptr``.


3.7. Variables should be initialized where they are declared.

    This ensures that variables are valid at any time. Sometimes it is impossible to
    initialize a variable to a valid value where it is declared.

    .. code-block:: c++

        int x, y, z;
        getCenter(&x, &y, &z);

    In these cases it should be left uninitialized rather than initialized to some phony
    value.

3.8. In most cases, class data members should not be declared ``public``.

    Public data members violate the concepts of information hiding and encapsulation.
    Use private variables and public accessor methods instead.

    Exceptions to this rule:

    * When the class is essentially a dumb data structure with no or minimal behavior
      (equivalent to a C struct, also known as POD type). In this case it is appropriate
      to make the instance variables public by using ``struct``.

    * When the class is used only inside the compilation unit, e.g., when implementing pImpl
      idiom (aka Bridge pattern) or similar cases.


3.9. C++ pointers and references should have their reference symbol next to the type rather
than to the name.

    .. code-block:: c++

        float* x; // NOT: float *x;
        int& y;   // NOT: int &y;

3.10. Implicit test for 0 should not be used other than for boolean variables and pointers.

    .. code-block:: c++

        if (nLines != 0)  // NOT: if (nLines)
        if (value != 0.0) // NOT: if (value)

3.11. *(removed)*

3.12. Loop variables should be initialized immediately before the loop.

    .. code-block:: c++

        bool isDone = false;   // NOT: bool isDone = false;
        while (!isDone) {      //      // other stuff
          ...                  //      while (!isDone) {
        }                      //        ...
                               //      }

3.13. The form ``while (true)`` should be used for infinite loops.

    .. code-block:: c++

        while (true) {
          ...
        }

        // NOT:
        for (;;) { // NO!
          :
        }
        while (1) { // NO!
          :
        }

3.14. Complex conditional expressions must be avoided. Introduce temporary boolean variables
instead.

    .. code-block:: c++

        bool isFinished = (elementNo < 0) || (elementNo > maxElement);
        bool isRepeatedEntry = elementNo == lastElement;
        if (isFinished || isRepeatedEntry) {
          ...
        }

        // NOT:
        // if ((elementNo < 0) || (elementNo > maxElement) || elementNo == lastElement) {
        //  ...
        // }

    By assigning boolean variables to expressions, the program gets automatic
    documentation.  The construction will be easier to read, debug, and maintain.

3.15. The conditional should be put on a separate line.

    .. code-block:: c++

        if (isDone)         // NOT: if (isDone) doCleanup();
          doCleanup();

    This is for debugging purposes. When writing on a single line, it is not apparent
    whether the test is really true or not.

3.16. Assignment statements in conditionals must be avoided.

    .. code-block:: c++

        File* fileHandle = open(fileName, "w");
        if (!fileHandle) {
          ...
        }

        // NOT
        // if (!(fileHandle = open(fileName, "w"))) {
        //  ..
        // }

3.17. The use of magic numbers in the code should be avoided. Numbers other than 0 and 1
should be considered declared as named constants instead.

    If the number does not have an obvious meaning by itself, the readability is enhanced
    by introducing a named constant instead. A different approach is to introduce a method
    from which the constant can be accessed.

3.18. Floating point literals should always be written with a decimal point, at least one
decimal, and without omitting 0 before the decimal point.

    .. code-block:: c++

        double total = 0.0;     // NOT: double total = 0;
        double someValue = 0.1; // NOT double someValue = .1;
        double speed = 3.0e8;   // NOT: double speed = 3e8;
        double sum;
        ...
        sum = (a + b) * 10.0;

3.19. ``goto`` should not be used.

    ``goto`` statements violate the idea of structured code. Only in very few cases (for
    instance, breaking out of deeply nested structures) should ``goto`` be considered,
    and only if the alternative structured counterpart is proven to be less readable.

3.20. ``nullptr`` should be used to represent a null pointer, instead of "0" or "NULL".

3.21. Logical units within a block should be separated by one blank line.

    .. code-block:: c++

        Matrix4x4 matrix = new Matrix4x4();

        double cosAngle = Math.cos(angle);
        double sinAngle = Math.sin(angle);

        matrix.setElement(1, 1, cosAngle);
        matrix.setElement(1, 2, sinAngle);
        matrix.setElement(2, 1, -sinAngle);
        matrix.setElement(2, 2, cosAngle);

        multiply(matrix);

    Enhance readability by introducing white space between logical units of a block.

3.22. Variables in declarations can be left aligned.

    .. code-block:: c++

        AsciiFile* file;
        int        nPoints;
        float      x, y;

    Enhance readability. The variables are easier to spot from the types by alignment.

3.23. Use alignment wherever it enhances readability.

    .. code-block:: c++

        value = (potential        * oilDensity)   / constant1 +
                (depth            * waterDensity) / constant2 +
                (zCoordinateValue * gasDensity)   / constant3;

        minPosition =     computeDistance(min,     x, y, z);
        averagePosition = computeDistance(average, x, y, z);

    There are a number of places in the code where white space can be included to enhance
    readability even if this violates common guidelines. Many of these cases have to do
    with code alignment. General guidelines on code alignment are difficult to give, but
    the examples above should give a general clue.

3.24. All comments should be written in English.

    In an international environment, English is the preferred language.

3.25. Use ``//`` for all comments, including multi-line comments.

    .. code-block:: c++

        // Comment spanning
        // more than one line.

    Since multilevel C-commenting is not supported, using ``//`` comments ensure that it
    is always possible to comment out entire sections of a file using ``/* */`` for
    debugging purposes etc.

    There should be a space between the ``//`` and the actual comment, and comments should
    always start with an upper case letter and end with a period.

    However, method and class documentation comments should use ``/** */`` style for
    Doxygen, JavaDoc and JSDoc. License boilerplate should use ``/* */`` style.

3.26. Comments should be included relative to their position in the code.

    .. code-block:: c++

        while (true) {
          // Do something
          something();
        }

        // NOT:
        while (true) {
        // Do something
          something();
        }

    This is to avoid that the comments break the logical structure of the program.

3.27. Use ``BOOST_ASSERT`` and ``BOOST_ASSERT_MSG`` for runtime assertions.

    .. code-block:: c++

        int x = 1;
        int y = 2;
        int z = x + y;
        BOOST_ASSERT(z - y == x);

    The expression passed to ``BOOST_ASSERT`` MUST NOT have side effects,
    because it MAY NOT be evaluated in release builds.

3.28. Use ``static_assert`` for static assertions.

    .. code-block:: c++

        class BaseClass
        {
        };

        class DerivedClass : public BaseClass
        {
        };

        static_assert(std::is_base_of<BaseClass, DerivedClass>::value,
                      "DerivedClass must inherit from BaseClass");

3.29. ``auto`` type specifier MAY be used for local variables, if a human reader
      can easily deduce the actual type.

    .. code-block:: c++

        std::vector<int> intVector;
        auto i = intVector.find(4);

        auto stringSet = std::make_shared<std::set<std::string>>();

    ``auto`` SHOULD NOT be used if a human reader cannot easily deduce the actual type.

    .. code-block:: c++

        auto x = foo(); // BAD if the declaration of foo() isn't nearby

    ``const auto&`` SHOULD be used to represent constant reference types.
    ``auto&&`` SHOULD be used to represent mutable reference types.

    .. code-block:: c++

        std::list<std::string> strings;
        for (const auto& str : strings) {
          statements; // cannot modify `str`
        }
        for (auto&& str : strings) {
          statements; // can modify `str`
        }

3.30. Use the ``override`` or ``final`` specifier when overriding a virtual
member function or a virtual destructor.

    ``virtual`` MUST NOT be used along with ``final``, so that the compiler
    can generate an error when a final function does not override.

    ``virtual`` SHOULD NOT be used along with ``override``, for consistency
    with ``final``.

    .. code-block:: c++

        class Stream
        {
        public:
          virtual void
          open();
        };

        class InputStream : public Stream
        {
        public:
          void
          open() override;
        };

        class Console : public InputStream
        {
        public:
          void
          open() final;
        };

3.31. The recommended way to throw an exception derived from ``std::exception`` is to use
the ``BOOST_THROW_EXCEPTION``
`macro <http://www.boost.org/doc/libs/1_54_0/libs/exception/doc/BOOST_THROW_EXCEPTION.html>`__.
Exceptions thrown using this macro will be augmented with additional diagnostic information,
including file name, line number, and function name from where the exception was thrown.
