CXX			= c++

NAME		= webserv

RM			= rm -rf

SRCS_DIR	= src

OBJS_DIR	= obj

CXXFLAGS	= -Wall -Wextra -Werror -MP -MD -std=c++98 -g

SRCS   	= \
        $(SRCS_DIR)/main.cpp \
        $(SRCS_DIR)/Config.cpp \
        $(SRCS_DIR)/Parser.cpp \
        $(SRCS_DIR)/Server.cpp \

OBJS	= $(SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o)

DEPS	= $(OBJS:%.o=%.d)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


all		: $(NAME)

run		: re
		@clear

fclean	: clean
		$(RM) $(NAME)
		$(RM) $(OBJS_DIR)

clean:
	$(RM) $(OBJS)

re		: fclean all

database: clean
	$(CXX) $(CXXFLAGS) -DDATABASE='"$(DB)"' -o $(NAME) $(SRCS)

.PHONY	: all clean fclean re run database