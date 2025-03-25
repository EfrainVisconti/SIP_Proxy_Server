USER_NAME      = Efrain Visconti

# Colors:
DEF_COLOR	   = \033[0;39m
BLUE		   = \033[0;94m
PURPLE         = \033[0;95m
GREEN		   = \033[0;92m

# Compiler and flags
CC             = c++
CFLAGS         = -Wall -Wextra -Werror -g3

RM             = rm -rf

# Directories
SRC_DIR        = srcs/
INC_DIR        = incs/
OBJ_DIR        = objs/

# Source files
SRC_FILE       = main.cpp ServerManager.cpp Socket.cpp SIP.cpp clients_utils.cpp utils.cpp
INC_FILE       = MCXServer.hpp

SRC            = $(addprefix $(SRC_DIR), $(SRC_FILE))
INC            = $(addprefix $(INC_DIR), $(INC_FILE))
OBJ_FILE       = $(SRC_FILE:.cpp=.o)
OBJ            = $(addprefix $(OBJ_DIR), $(OBJ_FILE))


# Output executable
NAME           = MCXServer

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ)
	@echo "Compiling $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)✔ $(BLUE)$(USER_NAME)'s $(PURPLE)$(NAME)$(BLUE) compilation$(DEF_COLOR)"

# Crear obj/ antes de compilar objetos
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compilar archivos fuente en objetos
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(GREEN)✔ $(BLUE)$(USER_NAME)'s $(PURPLE)$(NAME)$(BLUE) .o files removal$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)✔ $(BLUE)$(USER_NAME)'s $(PURPLE)$(NAME)$(BLUE) executable file removal$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re