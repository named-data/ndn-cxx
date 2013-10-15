# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# if !defined(NDNBOOST_PP_LOCAL_LIMITS)
#    error NDNBOOST_PP_ERROR:  local iteration boundaries are not defined
# elif !defined(NDNBOOST_PP_LOCAL_MACRO)
#    error NDNBOOST_PP_ERROR:  local iteration target macro is not defined
# else
#    if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#        define NDNBOOST_PP_LOCAL_S NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_LOCAL_LIMITS)
#        define NDNBOOST_PP_LOCAL_F NDNBOOST_PP_TUPLE_ELEM(2, 1, NDNBOOST_PP_LOCAL_LIMITS)
#    else
#        define NDNBOOST_PP_VALUE NDNBOOST_PP_TUPLE_ELEM(2, 0, NDNBOOST_PP_LOCAL_LIMITS)
#        include <ndnboost/preprocessor/iteration/detail/start.hpp>
#        define NDNBOOST_PP_VALUE NDNBOOST_PP_TUPLE_ELEM(2, 1, NDNBOOST_PP_LOCAL_LIMITS)
#        include <ndnboost/preprocessor/iteration/detail/finish.hpp>
#        define NDNBOOST_PP_LOCAL_S NDNBOOST_PP_LOCAL_SE()
#        define NDNBOOST_PP_LOCAL_F NDNBOOST_PP_LOCAL_FE()
#    endif
# endif
#
# if (NDNBOOST_PP_LOCAL_S) > (NDNBOOST_PP_LOCAL_F)
#    include <ndnboost/preprocessor/iteration/detail/rlocal.hpp>
# else
#    if NDNBOOST_PP_LOCAL_C(0)
        NDNBOOST_PP_LOCAL_MACRO(0)
#    endif
#    if NDNBOOST_PP_LOCAL_C(1)
        NDNBOOST_PP_LOCAL_MACRO(1)
#    endif
#    if NDNBOOST_PP_LOCAL_C(2)
        NDNBOOST_PP_LOCAL_MACRO(2)
#    endif
#    if NDNBOOST_PP_LOCAL_C(3)
        NDNBOOST_PP_LOCAL_MACRO(3)
#    endif
#    if NDNBOOST_PP_LOCAL_C(4)
        NDNBOOST_PP_LOCAL_MACRO(4)
#    endif
#    if NDNBOOST_PP_LOCAL_C(5)
        NDNBOOST_PP_LOCAL_MACRO(5)
#    endif
#    if NDNBOOST_PP_LOCAL_C(6)
        NDNBOOST_PP_LOCAL_MACRO(6)
#    endif
#    if NDNBOOST_PP_LOCAL_C(7)
        NDNBOOST_PP_LOCAL_MACRO(7)
#    endif
#    if NDNBOOST_PP_LOCAL_C(8)
        NDNBOOST_PP_LOCAL_MACRO(8)
#    endif
#    if NDNBOOST_PP_LOCAL_C(9)
        NDNBOOST_PP_LOCAL_MACRO(9)
#    endif
#    if NDNBOOST_PP_LOCAL_C(10)
        NDNBOOST_PP_LOCAL_MACRO(10)
#    endif
#    if NDNBOOST_PP_LOCAL_C(11)
        NDNBOOST_PP_LOCAL_MACRO(11)
#    endif
#    if NDNBOOST_PP_LOCAL_C(12)
        NDNBOOST_PP_LOCAL_MACRO(12)
#    endif
#    if NDNBOOST_PP_LOCAL_C(13)
        NDNBOOST_PP_LOCAL_MACRO(13)
#    endif
#    if NDNBOOST_PP_LOCAL_C(14)
        NDNBOOST_PP_LOCAL_MACRO(14)
#    endif
#    if NDNBOOST_PP_LOCAL_C(15)
        NDNBOOST_PP_LOCAL_MACRO(15)
#    endif
#    if NDNBOOST_PP_LOCAL_C(16)
        NDNBOOST_PP_LOCAL_MACRO(16)
#    endif
#    if NDNBOOST_PP_LOCAL_C(17)
        NDNBOOST_PP_LOCAL_MACRO(17)
#    endif
#    if NDNBOOST_PP_LOCAL_C(18)
        NDNBOOST_PP_LOCAL_MACRO(18)
#    endif
#    if NDNBOOST_PP_LOCAL_C(19)
        NDNBOOST_PP_LOCAL_MACRO(19)
#    endif
#    if NDNBOOST_PP_LOCAL_C(20)
        NDNBOOST_PP_LOCAL_MACRO(20)
#    endif
#    if NDNBOOST_PP_LOCAL_C(21)
        NDNBOOST_PP_LOCAL_MACRO(21)
#    endif
#    if NDNBOOST_PP_LOCAL_C(22)
        NDNBOOST_PP_LOCAL_MACRO(22)
#    endif
#    if NDNBOOST_PP_LOCAL_C(23)
        NDNBOOST_PP_LOCAL_MACRO(23)
#    endif
#    if NDNBOOST_PP_LOCAL_C(24)
        NDNBOOST_PP_LOCAL_MACRO(24)
#    endif
#    if NDNBOOST_PP_LOCAL_C(25)
        NDNBOOST_PP_LOCAL_MACRO(25)
#    endif
#    if NDNBOOST_PP_LOCAL_C(26)
        NDNBOOST_PP_LOCAL_MACRO(26)
#    endif
#    if NDNBOOST_PP_LOCAL_C(27)
        NDNBOOST_PP_LOCAL_MACRO(27)
#    endif
#    if NDNBOOST_PP_LOCAL_C(28)
        NDNBOOST_PP_LOCAL_MACRO(28)
#    endif
#    if NDNBOOST_PP_LOCAL_C(29)
        NDNBOOST_PP_LOCAL_MACRO(29)
#    endif
#    if NDNBOOST_PP_LOCAL_C(30)
        NDNBOOST_PP_LOCAL_MACRO(30)
#    endif
#    if NDNBOOST_PP_LOCAL_C(31)
        NDNBOOST_PP_LOCAL_MACRO(31)
#    endif
#    if NDNBOOST_PP_LOCAL_C(32)
        NDNBOOST_PP_LOCAL_MACRO(32)
#    endif
#    if NDNBOOST_PP_LOCAL_C(33)
        NDNBOOST_PP_LOCAL_MACRO(33)
#    endif
#    if NDNBOOST_PP_LOCAL_C(34)
        NDNBOOST_PP_LOCAL_MACRO(34)
#    endif
#    if NDNBOOST_PP_LOCAL_C(35)
        NDNBOOST_PP_LOCAL_MACRO(35)
#    endif
#    if NDNBOOST_PP_LOCAL_C(36)
        NDNBOOST_PP_LOCAL_MACRO(36)
#    endif
#    if NDNBOOST_PP_LOCAL_C(37)
        NDNBOOST_PP_LOCAL_MACRO(37)
#    endif
#    if NDNBOOST_PP_LOCAL_C(38)
        NDNBOOST_PP_LOCAL_MACRO(38)
#    endif
#    if NDNBOOST_PP_LOCAL_C(39)
        NDNBOOST_PP_LOCAL_MACRO(39)
#    endif
#    if NDNBOOST_PP_LOCAL_C(40)
        NDNBOOST_PP_LOCAL_MACRO(40)
#    endif
#    if NDNBOOST_PP_LOCAL_C(41)
        NDNBOOST_PP_LOCAL_MACRO(41)
#    endif
#    if NDNBOOST_PP_LOCAL_C(42)
        NDNBOOST_PP_LOCAL_MACRO(42)
#    endif
#    if NDNBOOST_PP_LOCAL_C(43)
        NDNBOOST_PP_LOCAL_MACRO(43)
#    endif
#    if NDNBOOST_PP_LOCAL_C(44)
        NDNBOOST_PP_LOCAL_MACRO(44)
#    endif
#    if NDNBOOST_PP_LOCAL_C(45)
        NDNBOOST_PP_LOCAL_MACRO(45)
#    endif
#    if NDNBOOST_PP_LOCAL_C(46)
        NDNBOOST_PP_LOCAL_MACRO(46)
#    endif
#    if NDNBOOST_PP_LOCAL_C(47)
        NDNBOOST_PP_LOCAL_MACRO(47)
#    endif
#    if NDNBOOST_PP_LOCAL_C(48)
        NDNBOOST_PP_LOCAL_MACRO(48)
#    endif
#    if NDNBOOST_PP_LOCAL_C(49)
        NDNBOOST_PP_LOCAL_MACRO(49)
#    endif
#    if NDNBOOST_PP_LOCAL_C(50)
        NDNBOOST_PP_LOCAL_MACRO(50)
#    endif
#    if NDNBOOST_PP_LOCAL_C(51)
        NDNBOOST_PP_LOCAL_MACRO(51)
#    endif
#    if NDNBOOST_PP_LOCAL_C(52)
        NDNBOOST_PP_LOCAL_MACRO(52)
#    endif
#    if NDNBOOST_PP_LOCAL_C(53)
        NDNBOOST_PP_LOCAL_MACRO(53)
#    endif
#    if NDNBOOST_PP_LOCAL_C(54)
        NDNBOOST_PP_LOCAL_MACRO(54)
#    endif
#    if NDNBOOST_PP_LOCAL_C(55)
        NDNBOOST_PP_LOCAL_MACRO(55)
#    endif
#    if NDNBOOST_PP_LOCAL_C(56)
        NDNBOOST_PP_LOCAL_MACRO(56)
#    endif
#    if NDNBOOST_PP_LOCAL_C(57)
        NDNBOOST_PP_LOCAL_MACRO(57)
#    endif
#    if NDNBOOST_PP_LOCAL_C(58)
        NDNBOOST_PP_LOCAL_MACRO(58)
#    endif
#    if NDNBOOST_PP_LOCAL_C(59)
        NDNBOOST_PP_LOCAL_MACRO(59)
#    endif
#    if NDNBOOST_PP_LOCAL_C(60)
        NDNBOOST_PP_LOCAL_MACRO(60)
#    endif
#    if NDNBOOST_PP_LOCAL_C(61)
        NDNBOOST_PP_LOCAL_MACRO(61)
#    endif
#    if NDNBOOST_PP_LOCAL_C(62)
        NDNBOOST_PP_LOCAL_MACRO(62)
#    endif
#    if NDNBOOST_PP_LOCAL_C(63)
        NDNBOOST_PP_LOCAL_MACRO(63)
#    endif
#    if NDNBOOST_PP_LOCAL_C(64)
        NDNBOOST_PP_LOCAL_MACRO(64)
#    endif
#    if NDNBOOST_PP_LOCAL_C(65)
        NDNBOOST_PP_LOCAL_MACRO(65)
#    endif
#    if NDNBOOST_PP_LOCAL_C(66)
        NDNBOOST_PP_LOCAL_MACRO(66)
#    endif
#    if NDNBOOST_PP_LOCAL_C(67)
        NDNBOOST_PP_LOCAL_MACRO(67)
#    endif
#    if NDNBOOST_PP_LOCAL_C(68)
        NDNBOOST_PP_LOCAL_MACRO(68)
#    endif
#    if NDNBOOST_PP_LOCAL_C(69)
        NDNBOOST_PP_LOCAL_MACRO(69)
#    endif
#    if NDNBOOST_PP_LOCAL_C(70)
        NDNBOOST_PP_LOCAL_MACRO(70)
#    endif
#    if NDNBOOST_PP_LOCAL_C(71)
        NDNBOOST_PP_LOCAL_MACRO(71)
#    endif
#    if NDNBOOST_PP_LOCAL_C(72)
        NDNBOOST_PP_LOCAL_MACRO(72)
#    endif
#    if NDNBOOST_PP_LOCAL_C(73)
        NDNBOOST_PP_LOCAL_MACRO(73)
#    endif
#    if NDNBOOST_PP_LOCAL_C(74)
        NDNBOOST_PP_LOCAL_MACRO(74)
#    endif
#    if NDNBOOST_PP_LOCAL_C(75)
        NDNBOOST_PP_LOCAL_MACRO(75)
#    endif
#    if NDNBOOST_PP_LOCAL_C(76)
        NDNBOOST_PP_LOCAL_MACRO(76)
#    endif
#    if NDNBOOST_PP_LOCAL_C(77)
        NDNBOOST_PP_LOCAL_MACRO(77)
#    endif
#    if NDNBOOST_PP_LOCAL_C(78)
        NDNBOOST_PP_LOCAL_MACRO(78)
#    endif
#    if NDNBOOST_PP_LOCAL_C(79)
        NDNBOOST_PP_LOCAL_MACRO(79)
#    endif
#    if NDNBOOST_PP_LOCAL_C(80)
        NDNBOOST_PP_LOCAL_MACRO(80)
#    endif
#    if NDNBOOST_PP_LOCAL_C(81)
        NDNBOOST_PP_LOCAL_MACRO(81)
#    endif
#    if NDNBOOST_PP_LOCAL_C(82)
        NDNBOOST_PP_LOCAL_MACRO(82)
#    endif
#    if NDNBOOST_PP_LOCAL_C(83)
        NDNBOOST_PP_LOCAL_MACRO(83)
#    endif
#    if NDNBOOST_PP_LOCAL_C(84)
        NDNBOOST_PP_LOCAL_MACRO(84)
#    endif
#    if NDNBOOST_PP_LOCAL_C(85)
        NDNBOOST_PP_LOCAL_MACRO(85)
#    endif
#    if NDNBOOST_PP_LOCAL_C(86)
        NDNBOOST_PP_LOCAL_MACRO(86)
#    endif
#    if NDNBOOST_PP_LOCAL_C(87)
        NDNBOOST_PP_LOCAL_MACRO(87)
#    endif
#    if NDNBOOST_PP_LOCAL_C(88)
        NDNBOOST_PP_LOCAL_MACRO(88)
#    endif
#    if NDNBOOST_PP_LOCAL_C(89)
        NDNBOOST_PP_LOCAL_MACRO(89)
#    endif
#    if NDNBOOST_PP_LOCAL_C(90)
        NDNBOOST_PP_LOCAL_MACRO(90)
#    endif
#    if NDNBOOST_PP_LOCAL_C(91)
        NDNBOOST_PP_LOCAL_MACRO(91)
#    endif
#    if NDNBOOST_PP_LOCAL_C(92)
        NDNBOOST_PP_LOCAL_MACRO(92)
#    endif
#    if NDNBOOST_PP_LOCAL_C(93)
        NDNBOOST_PP_LOCAL_MACRO(93)
#    endif
#    if NDNBOOST_PP_LOCAL_C(94)
        NDNBOOST_PP_LOCAL_MACRO(94)
#    endif
#    if NDNBOOST_PP_LOCAL_C(95)
        NDNBOOST_PP_LOCAL_MACRO(95)
#    endif
#    if NDNBOOST_PP_LOCAL_C(96)
        NDNBOOST_PP_LOCAL_MACRO(96)
#    endif
#    if NDNBOOST_PP_LOCAL_C(97)
        NDNBOOST_PP_LOCAL_MACRO(97)
#    endif
#    if NDNBOOST_PP_LOCAL_C(98)
        NDNBOOST_PP_LOCAL_MACRO(98)
#    endif
#    if NDNBOOST_PP_LOCAL_C(99)
        NDNBOOST_PP_LOCAL_MACRO(99)
#    endif
#    if NDNBOOST_PP_LOCAL_C(100)
        NDNBOOST_PP_LOCAL_MACRO(100)
#    endif
#    if NDNBOOST_PP_LOCAL_C(101)
        NDNBOOST_PP_LOCAL_MACRO(101)
#    endif
#    if NDNBOOST_PP_LOCAL_C(102)
        NDNBOOST_PP_LOCAL_MACRO(102)
#    endif
#    if NDNBOOST_PP_LOCAL_C(103)
        NDNBOOST_PP_LOCAL_MACRO(103)
#    endif
#    if NDNBOOST_PP_LOCAL_C(104)
        NDNBOOST_PP_LOCAL_MACRO(104)
#    endif
#    if NDNBOOST_PP_LOCAL_C(105)
        NDNBOOST_PP_LOCAL_MACRO(105)
#    endif
#    if NDNBOOST_PP_LOCAL_C(106)
        NDNBOOST_PP_LOCAL_MACRO(106)
#    endif
#    if NDNBOOST_PP_LOCAL_C(107)
        NDNBOOST_PP_LOCAL_MACRO(107)
#    endif
#    if NDNBOOST_PP_LOCAL_C(108)
        NDNBOOST_PP_LOCAL_MACRO(108)
#    endif
#    if NDNBOOST_PP_LOCAL_C(109)
        NDNBOOST_PP_LOCAL_MACRO(109)
#    endif
#    if NDNBOOST_PP_LOCAL_C(110)
        NDNBOOST_PP_LOCAL_MACRO(110)
#    endif
#    if NDNBOOST_PP_LOCAL_C(111)
        NDNBOOST_PP_LOCAL_MACRO(111)
#    endif
#    if NDNBOOST_PP_LOCAL_C(112)
        NDNBOOST_PP_LOCAL_MACRO(112)
#    endif
#    if NDNBOOST_PP_LOCAL_C(113)
        NDNBOOST_PP_LOCAL_MACRO(113)
#    endif
#    if NDNBOOST_PP_LOCAL_C(114)
        NDNBOOST_PP_LOCAL_MACRO(114)
#    endif
#    if NDNBOOST_PP_LOCAL_C(115)
        NDNBOOST_PP_LOCAL_MACRO(115)
#    endif
#    if NDNBOOST_PP_LOCAL_C(116)
        NDNBOOST_PP_LOCAL_MACRO(116)
#    endif
#    if NDNBOOST_PP_LOCAL_C(117)
        NDNBOOST_PP_LOCAL_MACRO(117)
#    endif
#    if NDNBOOST_PP_LOCAL_C(118)
        NDNBOOST_PP_LOCAL_MACRO(118)
#    endif
#    if NDNBOOST_PP_LOCAL_C(119)
        NDNBOOST_PP_LOCAL_MACRO(119)
#    endif
#    if NDNBOOST_PP_LOCAL_C(120)
        NDNBOOST_PP_LOCAL_MACRO(120)
#    endif
#    if NDNBOOST_PP_LOCAL_C(121)
        NDNBOOST_PP_LOCAL_MACRO(121)
#    endif
#    if NDNBOOST_PP_LOCAL_C(122)
        NDNBOOST_PP_LOCAL_MACRO(122)
#    endif
#    if NDNBOOST_PP_LOCAL_C(123)
        NDNBOOST_PP_LOCAL_MACRO(123)
#    endif
#    if NDNBOOST_PP_LOCAL_C(124)
        NDNBOOST_PP_LOCAL_MACRO(124)
#    endif
#    if NDNBOOST_PP_LOCAL_C(125)
        NDNBOOST_PP_LOCAL_MACRO(125)
#    endif
#    if NDNBOOST_PP_LOCAL_C(126)
        NDNBOOST_PP_LOCAL_MACRO(126)
#    endif
#    if NDNBOOST_PP_LOCAL_C(127)
        NDNBOOST_PP_LOCAL_MACRO(127)
#    endif
#    if NDNBOOST_PP_LOCAL_C(128)
        NDNBOOST_PP_LOCAL_MACRO(128)
#    endif
#    if NDNBOOST_PP_LOCAL_C(129)
        NDNBOOST_PP_LOCAL_MACRO(129)
#    endif
#    if NDNBOOST_PP_LOCAL_C(130)
        NDNBOOST_PP_LOCAL_MACRO(130)
#    endif
#    if NDNBOOST_PP_LOCAL_C(131)
        NDNBOOST_PP_LOCAL_MACRO(131)
#    endif
#    if NDNBOOST_PP_LOCAL_C(132)
        NDNBOOST_PP_LOCAL_MACRO(132)
#    endif
#    if NDNBOOST_PP_LOCAL_C(133)
        NDNBOOST_PP_LOCAL_MACRO(133)
#    endif
#    if NDNBOOST_PP_LOCAL_C(134)
        NDNBOOST_PP_LOCAL_MACRO(134)
#    endif
#    if NDNBOOST_PP_LOCAL_C(135)
        NDNBOOST_PP_LOCAL_MACRO(135)
#    endif
#    if NDNBOOST_PP_LOCAL_C(136)
        NDNBOOST_PP_LOCAL_MACRO(136)
#    endif
#    if NDNBOOST_PP_LOCAL_C(137)
        NDNBOOST_PP_LOCAL_MACRO(137)
#    endif
#    if NDNBOOST_PP_LOCAL_C(138)
        NDNBOOST_PP_LOCAL_MACRO(138)
#    endif
#    if NDNBOOST_PP_LOCAL_C(139)
        NDNBOOST_PP_LOCAL_MACRO(139)
#    endif
#    if NDNBOOST_PP_LOCAL_C(140)
        NDNBOOST_PP_LOCAL_MACRO(140)
#    endif
#    if NDNBOOST_PP_LOCAL_C(141)
        NDNBOOST_PP_LOCAL_MACRO(141)
#    endif
#    if NDNBOOST_PP_LOCAL_C(142)
        NDNBOOST_PP_LOCAL_MACRO(142)
#    endif
#    if NDNBOOST_PP_LOCAL_C(143)
        NDNBOOST_PP_LOCAL_MACRO(143)
#    endif
#    if NDNBOOST_PP_LOCAL_C(144)
        NDNBOOST_PP_LOCAL_MACRO(144)
#    endif
#    if NDNBOOST_PP_LOCAL_C(145)
        NDNBOOST_PP_LOCAL_MACRO(145)
#    endif
#    if NDNBOOST_PP_LOCAL_C(146)
        NDNBOOST_PP_LOCAL_MACRO(146)
#    endif
#    if NDNBOOST_PP_LOCAL_C(147)
        NDNBOOST_PP_LOCAL_MACRO(147)
#    endif
#    if NDNBOOST_PP_LOCAL_C(148)
        NDNBOOST_PP_LOCAL_MACRO(148)
#    endif
#    if NDNBOOST_PP_LOCAL_C(149)
        NDNBOOST_PP_LOCAL_MACRO(149)
#    endif
#    if NDNBOOST_PP_LOCAL_C(150)
        NDNBOOST_PP_LOCAL_MACRO(150)
#    endif
#    if NDNBOOST_PP_LOCAL_C(151)
        NDNBOOST_PP_LOCAL_MACRO(151)
#    endif
#    if NDNBOOST_PP_LOCAL_C(152)
        NDNBOOST_PP_LOCAL_MACRO(152)
#    endif
#    if NDNBOOST_PP_LOCAL_C(153)
        NDNBOOST_PP_LOCAL_MACRO(153)
#    endif
#    if NDNBOOST_PP_LOCAL_C(154)
        NDNBOOST_PP_LOCAL_MACRO(154)
#    endif
#    if NDNBOOST_PP_LOCAL_C(155)
        NDNBOOST_PP_LOCAL_MACRO(155)
#    endif
#    if NDNBOOST_PP_LOCAL_C(156)
        NDNBOOST_PP_LOCAL_MACRO(156)
#    endif
#    if NDNBOOST_PP_LOCAL_C(157)
        NDNBOOST_PP_LOCAL_MACRO(157)
#    endif
#    if NDNBOOST_PP_LOCAL_C(158)
        NDNBOOST_PP_LOCAL_MACRO(158)
#    endif
#    if NDNBOOST_PP_LOCAL_C(159)
        NDNBOOST_PP_LOCAL_MACRO(159)
#    endif
#    if NDNBOOST_PP_LOCAL_C(160)
        NDNBOOST_PP_LOCAL_MACRO(160)
#    endif
#    if NDNBOOST_PP_LOCAL_C(161)
        NDNBOOST_PP_LOCAL_MACRO(161)
#    endif
#    if NDNBOOST_PP_LOCAL_C(162)
        NDNBOOST_PP_LOCAL_MACRO(162)
#    endif
#    if NDNBOOST_PP_LOCAL_C(163)
        NDNBOOST_PP_LOCAL_MACRO(163)
#    endif
#    if NDNBOOST_PP_LOCAL_C(164)
        NDNBOOST_PP_LOCAL_MACRO(164)
#    endif
#    if NDNBOOST_PP_LOCAL_C(165)
        NDNBOOST_PP_LOCAL_MACRO(165)
#    endif
#    if NDNBOOST_PP_LOCAL_C(166)
        NDNBOOST_PP_LOCAL_MACRO(166)
#    endif
#    if NDNBOOST_PP_LOCAL_C(167)
        NDNBOOST_PP_LOCAL_MACRO(167)
#    endif
#    if NDNBOOST_PP_LOCAL_C(168)
        NDNBOOST_PP_LOCAL_MACRO(168)
#    endif
#    if NDNBOOST_PP_LOCAL_C(169)
        NDNBOOST_PP_LOCAL_MACRO(169)
#    endif
#    if NDNBOOST_PP_LOCAL_C(170)
        NDNBOOST_PP_LOCAL_MACRO(170)
#    endif
#    if NDNBOOST_PP_LOCAL_C(171)
        NDNBOOST_PP_LOCAL_MACRO(171)
#    endif
#    if NDNBOOST_PP_LOCAL_C(172)
        NDNBOOST_PP_LOCAL_MACRO(172)
#    endif
#    if NDNBOOST_PP_LOCAL_C(173)
        NDNBOOST_PP_LOCAL_MACRO(173)
#    endif
#    if NDNBOOST_PP_LOCAL_C(174)
        NDNBOOST_PP_LOCAL_MACRO(174)
#    endif
#    if NDNBOOST_PP_LOCAL_C(175)
        NDNBOOST_PP_LOCAL_MACRO(175)
#    endif
#    if NDNBOOST_PP_LOCAL_C(176)
        NDNBOOST_PP_LOCAL_MACRO(176)
#    endif
#    if NDNBOOST_PP_LOCAL_C(177)
        NDNBOOST_PP_LOCAL_MACRO(177)
#    endif
#    if NDNBOOST_PP_LOCAL_C(178)
        NDNBOOST_PP_LOCAL_MACRO(178)
#    endif
#    if NDNBOOST_PP_LOCAL_C(179)
        NDNBOOST_PP_LOCAL_MACRO(179)
#    endif
#    if NDNBOOST_PP_LOCAL_C(180)
        NDNBOOST_PP_LOCAL_MACRO(180)
#    endif
#    if NDNBOOST_PP_LOCAL_C(181)
        NDNBOOST_PP_LOCAL_MACRO(181)
#    endif
#    if NDNBOOST_PP_LOCAL_C(182)
        NDNBOOST_PP_LOCAL_MACRO(182)
#    endif
#    if NDNBOOST_PP_LOCAL_C(183)
        NDNBOOST_PP_LOCAL_MACRO(183)
#    endif
#    if NDNBOOST_PP_LOCAL_C(184)
        NDNBOOST_PP_LOCAL_MACRO(184)
#    endif
#    if NDNBOOST_PP_LOCAL_C(185)
        NDNBOOST_PP_LOCAL_MACRO(185)
#    endif
#    if NDNBOOST_PP_LOCAL_C(186)
        NDNBOOST_PP_LOCAL_MACRO(186)
#    endif
#    if NDNBOOST_PP_LOCAL_C(187)
        NDNBOOST_PP_LOCAL_MACRO(187)
#    endif
#    if NDNBOOST_PP_LOCAL_C(188)
        NDNBOOST_PP_LOCAL_MACRO(188)
#    endif
#    if NDNBOOST_PP_LOCAL_C(189)
        NDNBOOST_PP_LOCAL_MACRO(189)
#    endif
#    if NDNBOOST_PP_LOCAL_C(190)
        NDNBOOST_PP_LOCAL_MACRO(190)
#    endif
#    if NDNBOOST_PP_LOCAL_C(191)
        NDNBOOST_PP_LOCAL_MACRO(191)
#    endif
#    if NDNBOOST_PP_LOCAL_C(192)
        NDNBOOST_PP_LOCAL_MACRO(192)
#    endif
#    if NDNBOOST_PP_LOCAL_C(193)
        NDNBOOST_PP_LOCAL_MACRO(193)
#    endif
#    if NDNBOOST_PP_LOCAL_C(194)
        NDNBOOST_PP_LOCAL_MACRO(194)
#    endif
#    if NDNBOOST_PP_LOCAL_C(195)
        NDNBOOST_PP_LOCAL_MACRO(195)
#    endif
#    if NDNBOOST_PP_LOCAL_C(196)
        NDNBOOST_PP_LOCAL_MACRO(196)
#    endif
#    if NDNBOOST_PP_LOCAL_C(197)
        NDNBOOST_PP_LOCAL_MACRO(197)
#    endif
#    if NDNBOOST_PP_LOCAL_C(198)
        NDNBOOST_PP_LOCAL_MACRO(198)
#    endif
#    if NDNBOOST_PP_LOCAL_C(199)
        NDNBOOST_PP_LOCAL_MACRO(199)
#    endif
#    if NDNBOOST_PP_LOCAL_C(200)
        NDNBOOST_PP_LOCAL_MACRO(200)
#    endif
#    if NDNBOOST_PP_LOCAL_C(201)
        NDNBOOST_PP_LOCAL_MACRO(201)
#    endif
#    if NDNBOOST_PP_LOCAL_C(202)
        NDNBOOST_PP_LOCAL_MACRO(202)
#    endif
#    if NDNBOOST_PP_LOCAL_C(203)
        NDNBOOST_PP_LOCAL_MACRO(203)
#    endif
#    if NDNBOOST_PP_LOCAL_C(204)
        NDNBOOST_PP_LOCAL_MACRO(204)
#    endif
#    if NDNBOOST_PP_LOCAL_C(205)
        NDNBOOST_PP_LOCAL_MACRO(205)
#    endif
#    if NDNBOOST_PP_LOCAL_C(206)
        NDNBOOST_PP_LOCAL_MACRO(206)
#    endif
#    if NDNBOOST_PP_LOCAL_C(207)
        NDNBOOST_PP_LOCAL_MACRO(207)
#    endif
#    if NDNBOOST_PP_LOCAL_C(208)
        NDNBOOST_PP_LOCAL_MACRO(208)
#    endif
#    if NDNBOOST_PP_LOCAL_C(209)
        NDNBOOST_PP_LOCAL_MACRO(209)
#    endif
#    if NDNBOOST_PP_LOCAL_C(210)
        NDNBOOST_PP_LOCAL_MACRO(210)
#    endif
#    if NDNBOOST_PP_LOCAL_C(211)
        NDNBOOST_PP_LOCAL_MACRO(211)
#    endif
#    if NDNBOOST_PP_LOCAL_C(212)
        NDNBOOST_PP_LOCAL_MACRO(212)
#    endif
#    if NDNBOOST_PP_LOCAL_C(213)
        NDNBOOST_PP_LOCAL_MACRO(213)
#    endif
#    if NDNBOOST_PP_LOCAL_C(214)
        NDNBOOST_PP_LOCAL_MACRO(214)
#    endif
#    if NDNBOOST_PP_LOCAL_C(215)
        NDNBOOST_PP_LOCAL_MACRO(215)
#    endif
#    if NDNBOOST_PP_LOCAL_C(216)
        NDNBOOST_PP_LOCAL_MACRO(216)
#    endif
#    if NDNBOOST_PP_LOCAL_C(217)
        NDNBOOST_PP_LOCAL_MACRO(217)
#    endif
#    if NDNBOOST_PP_LOCAL_C(218)
        NDNBOOST_PP_LOCAL_MACRO(218)
#    endif
#    if NDNBOOST_PP_LOCAL_C(219)
        NDNBOOST_PP_LOCAL_MACRO(219)
#    endif
#    if NDNBOOST_PP_LOCAL_C(220)
        NDNBOOST_PP_LOCAL_MACRO(220)
#    endif
#    if NDNBOOST_PP_LOCAL_C(221)
        NDNBOOST_PP_LOCAL_MACRO(221)
#    endif
#    if NDNBOOST_PP_LOCAL_C(222)
        NDNBOOST_PP_LOCAL_MACRO(222)
#    endif
#    if NDNBOOST_PP_LOCAL_C(223)
        NDNBOOST_PP_LOCAL_MACRO(223)
#    endif
#    if NDNBOOST_PP_LOCAL_C(224)
        NDNBOOST_PP_LOCAL_MACRO(224)
#    endif
#    if NDNBOOST_PP_LOCAL_C(225)
        NDNBOOST_PP_LOCAL_MACRO(225)
#    endif
#    if NDNBOOST_PP_LOCAL_C(226)
        NDNBOOST_PP_LOCAL_MACRO(226)
#    endif
#    if NDNBOOST_PP_LOCAL_C(227)
        NDNBOOST_PP_LOCAL_MACRO(227)
#    endif
#    if NDNBOOST_PP_LOCAL_C(228)
        NDNBOOST_PP_LOCAL_MACRO(228)
#    endif
#    if NDNBOOST_PP_LOCAL_C(229)
        NDNBOOST_PP_LOCAL_MACRO(229)
#    endif
#    if NDNBOOST_PP_LOCAL_C(230)
        NDNBOOST_PP_LOCAL_MACRO(230)
#    endif
#    if NDNBOOST_PP_LOCAL_C(231)
        NDNBOOST_PP_LOCAL_MACRO(231)
#    endif
#    if NDNBOOST_PP_LOCAL_C(232)
        NDNBOOST_PP_LOCAL_MACRO(232)
#    endif
#    if NDNBOOST_PP_LOCAL_C(233)
        NDNBOOST_PP_LOCAL_MACRO(233)
#    endif
#    if NDNBOOST_PP_LOCAL_C(234)
        NDNBOOST_PP_LOCAL_MACRO(234)
#    endif
#    if NDNBOOST_PP_LOCAL_C(235)
        NDNBOOST_PP_LOCAL_MACRO(235)
#    endif
#    if NDNBOOST_PP_LOCAL_C(236)
        NDNBOOST_PP_LOCAL_MACRO(236)
#    endif

#    if NDNBOOST_PP_LOCAL_C(237)
        NDNBOOST_PP_LOCAL_MACRO(237)
#    endif
#    if NDNBOOST_PP_LOCAL_C(238)
        NDNBOOST_PP_LOCAL_MACRO(238)
#    endif
#    if NDNBOOST_PP_LOCAL_C(239)
        NDNBOOST_PP_LOCAL_MACRO(239)
#    endif
#    if NDNBOOST_PP_LOCAL_C(240)
        NDNBOOST_PP_LOCAL_MACRO(240)
#    endif
#    if NDNBOOST_PP_LOCAL_C(241)
        NDNBOOST_PP_LOCAL_MACRO(241)
#    endif
#    if NDNBOOST_PP_LOCAL_C(242)
        NDNBOOST_PP_LOCAL_MACRO(242)
#    endif
#    if NDNBOOST_PP_LOCAL_C(243)
        NDNBOOST_PP_LOCAL_MACRO(243)
#    endif
#    if NDNBOOST_PP_LOCAL_C(244)
        NDNBOOST_PP_LOCAL_MACRO(244)
#    endif
#    if NDNBOOST_PP_LOCAL_C(245)
        NDNBOOST_PP_LOCAL_MACRO(245)
#    endif
#    if NDNBOOST_PP_LOCAL_C(246)
        NDNBOOST_PP_LOCAL_MACRO(246)
#    endif
#    if NDNBOOST_PP_LOCAL_C(247)
        NDNBOOST_PP_LOCAL_MACRO(247)
#    endif
#    if NDNBOOST_PP_LOCAL_C(248)
        NDNBOOST_PP_LOCAL_MACRO(248)
#    endif
#    if NDNBOOST_PP_LOCAL_C(249)
        NDNBOOST_PP_LOCAL_MACRO(249)
#    endif
#    if NDNBOOST_PP_LOCAL_C(250)
        NDNBOOST_PP_LOCAL_MACRO(250)
#    endif
#    if NDNBOOST_PP_LOCAL_C(251)
        NDNBOOST_PP_LOCAL_MACRO(251)
#    endif
#    if NDNBOOST_PP_LOCAL_C(252)
        NDNBOOST_PP_LOCAL_MACRO(252)
#    endif
#    if NDNBOOST_PP_LOCAL_C(253)
        NDNBOOST_PP_LOCAL_MACRO(253)
#    endif
#    if NDNBOOST_PP_LOCAL_C(254)
        NDNBOOST_PP_LOCAL_MACRO(254)
#    endif
#    if NDNBOOST_PP_LOCAL_C(255)
        NDNBOOST_PP_LOCAL_MACRO(255)
#    endif
#    if NDNBOOST_PP_LOCAL_C(256)
        NDNBOOST_PP_LOCAL_MACRO(256)
#    endif
# endif
#
# undef NDNBOOST_PP_LOCAL_LIMITS
#
# undef NDNBOOST_PP_LOCAL_S
# undef NDNBOOST_PP_LOCAL_F
#
# undef NDNBOOST_PP_LOCAL_MACRO
