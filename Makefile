CXX			= c++

NAME		= webserv

RM			= rm -rf

SRCS_DIR	= src

OBJS_DIR	= obj

LDFLAGS     = -pthread
GTEST_DIR   = googletest

CXXFLAGS	= -Wall -Wextra -Werror -MP -MD -std=c++98 -g -Iinc -I$(GTEST_DIR)/include


TESTFLAGS	= -Wall -Wextra -Werror -MP -MD -g -Iinc -I$(GTEST_DIR)/include

SRCS   	= \
		$(SRCS_DIR)/Node.cpp \
		$(SRCS_DIR)/Request.cpp \
		$(SRCS_DIR)/ClientInfo.cpp \
		$(SRCS_DIR)/LocationInfo.cpp \
		$(SRCS_DIR)/ServerInfo.cpp \
		$(SRCS_DIR)/Log.cpp \
		$(SRCS_DIR)/Director.cpp \
        $(SRCS_DIR)/Config.cpp \
        $(SRCS_DIR)/ConfigParser.cpp \
        $(SRCS_DIR)/ConfigDispatcher.cpp \
        $(SRCS_DIR)/Utils.cpp \

OBJS	= $(SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o)

DEPS	= $(OBJS:%.o=%.d)

TESTS_DIR = test
TEST_SRCS	= $(wildcard $(TESTS_DIR)/*.cpp)
TEST_OBJS	= $(TEST_SRCS:%.cpp=%.o)
TEST_EXEC	= testik

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_EXEC): $(TEST_OBJS) $(OBJS)
	$(CXX) $(TESTFLAGS) $(LDFLAGS) -o $(TEST_EXEC) $(TEST_OBJS) $(OBJS) -L$(GTEST_DIR)/lib -lgtest -lgtest_main

$(TESTS_DIR)/%.o: $(TESTS_DIR)/%.cpp
	$(CXX) $(TESTFLAGS) -c $< -o $@

all		: $(NAME)

run_tests: $(TEST_EXEC)
	./$(TEST_EXEC)

fclean	: clean
		$(RM) $(NAME)
		$(RM) $(OBJS_DIR)
		$(RM) webserv.d

clean:
	$(RM) $(OBJS)

re		: fclean all

.PHONY	: all clean fclean re run
