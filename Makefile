CXX			= c++

NAME		= webserv

RM			= rm -rf

SRCS_DIR	= src

OBJS_DIR	= obj

CXXFLAGS	= -Wall -Wextra -Werror -MP -MD -std=c++98 -g -Iinc

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
        $(SRCS_DIR)/main3.cpp \

OBJS	= $(SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o)

DEPS	= $(OBJS:%.o=%.d)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


all		: $(NAME)

run		: re


fclean	: clean
		$(RM) $(NAME)
		$(RM) $(OBJS_DIR)
		$(RM) webserv.d

clean:
	$(RM) $(OBJS)

re		: fclean all

.PHONY	: all clean fclean re run
