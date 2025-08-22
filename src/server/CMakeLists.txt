# 当前目录下所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_SRC_LIST)

# 生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_SRC_LIST})

# 连接依赖库
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)