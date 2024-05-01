CXX			= c++

NAME		= webserv

RM			= rm -rf

SRCS_DIR	= src

OBJS_DIR	= obj

CXXFLAGS	= -Wall -Wextra -Werror -MP -MD -std=c++98 -Iinc

SRCS   	= \
		$(SRCS_DIR)/CGI.cpp \
		$(SRCS_DIR)/Server.cpp \
		$(SRCS_DIR)/Log.cpp \
		$(SRCS_DIR)/Request.cpp \
		$(SRCS_DIR)/Director.cpp \
        $(SRCS_DIR)/Config.cpp \
        $(SRCS_DIR)/ConfigParser.cpp \
        $(SRCS_DIR)/ConfigSetters.cpp \
        $(SRCS_DIR)/Utils.cpp \
		$(SRCS_DIR)/main.cpp

OBJS		= $(SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o)

DEPS	= $(OBJS:%.o=%.d)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)  -lpthread

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all		: $(NAME)

fclean	: clean
		$(RM) ./logs
		$(RM) ./error_pages
		$(RM) $(NAME)
		$(RM) $(TEST_OBJS_DIR)
		$(RM) $(OBJS_DIR)
		$(RM) webserv.d

clean:
	$(RM) $(OBJS)

re		: fclean all

-include $(DEPS)

.PHONY	: all clean fclean re
