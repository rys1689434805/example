
#include <stdio.h>
#include <string.h>

#include <mysql.h>


#define MYSQL_SERVER 		"192.168.189.134"
#define MYSQL_USERNAME		"replication"
#define MYSQL_PASSWORD		"123456"
#define MYSQL_DBNAME		"vip_order"

#define BINARY_LENGTH		256*1024

#define MYSQL_SELECT_TBL_USER	"SELECT U_IMG FROM TBL_USER"
#define MYSQL_INSERT_TBL_USER	"INSERT TBL_USER(U_NAME, U_GENDER, U_IMG) VALUES('BoJing', 'man', ?)" //?表示占位用于二进制数据

int read_binary(char *filename, char *buffer) {

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("fopen failed\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);//文件指针从0开始偏移到文件尾
	int length = ftell(fp);//获得文件指针当前位置
	fseek(fp, 0, SEEK_SET);
	
	int size = fread(buffer, 1, length, fp);//每次读一个字节读length次
	if (size != length) {
		printf("fread faile\n");
		return size;
	}

	fclose(fp);

	return size;
}

int write_binary(char *filename, char *buffer, int length) {

	FILE *fp = fopen(filename, "wb+");
	if (fp == NULL) {
		printf("fopen failed\n");
		return -1;
	}
	
	int size = fwrite(buffer, 1, length, fp);
	if (size != length) {
		printf("fread faile\n");
		return size;
	}

	fclose(fp);

	return size;
}

//
int mysql_write(MYSQL *client, char *buffer, long length) {

	if (client == NULL) return -1;
	if (buffer == NULL) return -2;
	if (length <= 0) return -3;

	MYSQL_STMT *stmt = mysql_stmt_init(client);
	int ret = mysql_stmt_prepare(stmt, MYSQL_INSERT_TBL_USER, strlen(MYSQL_INSERT_TBL_USER));

	MYSQL_BIND param = {0};
	param.buffer_type = MYSQL_TYPE_LONG_BLOB;
	param.buffer = NULL;
	param.is_null = 0;
	param.length = NULL;

	ret = mysql_stmt_bind_param(stmt, &param);

	if (mysql_stmt_send_long_data(stmt, 0, buffer, length)) { //将为二进制数据写入mysql
		printf(" err : %s\n", mysql_error(client));
	}

	ret = mysql_stmt_execute(stmt);
	mysql_stmt_close(stmt);
	
}


//
int mysql_read(MYSQL *client, char *buffer, int length) {
	if (client == NULL) return -1;
	if (buffer == NULL) return -2;
	if (length <= 0) return -3;

	
	MYSQL_STMT *stmt = mysql_stmt_init(client);
	int ret = mysql_stmt_prepare(stmt, MYSQL_SELECT_TBL_USER, strlen(MYSQL_SELECT_TBL_USER));
	
	MYSQL_BIND result = {0};
	unsigned long total_length = 0;
	
	result.buffer_type = MYSQL_TYPE_LONG_BLOB;
	result.length = &total_length;

	ret = mysql_stmt_bind_result(stmt, &result);
	ret = mysql_stmt_execute(stmt);
	ret = mysql_stmt_store_result(stmt);

	for (;;) {

		ret = mysql_stmt_fetch(stmt);//取出stmt
		if (ret != 0 && ret != MYSQL_DATA_TRUNCATED) break;

		int start = 0;
		while (start < (int)total_length) {

			result.buffer = buffer+start;
			result.buffer_length = 1;

			ret = mysql_stmt_fetch_column(stmt, &result, 0, start);//读取二进制数据
            if (ret!=0)
            {
                printf("msg=%s", mysql_error(client));
                return;
            }
            start += result.buffer_length;
		}

	}

	mysql_stmt_close(stmt);

	return total_length;
	
}


int main() {

	MYSQL mysql;
	if (NULL == mysql_init(&mysql)) {
		printf("mysql_init : %s\n", mysql_error(&mysql));
		return -1;
	}

	//mysql_connect
	if (!mysql_real_connect(&mysql, MYSQL_SERVER, MYSQL_USERNAME, MYSQL_PASSWORD, 
		MYSQL_DBNAME, 0, NULL, 0)) {
		printf("mysql_real_connect : %s\n", mysql_error(&mysql));
		return -1;
	}

	printf("Connected MySQL succcessful!");

	char buffer[BINARY_LENGTH] = {0};
	int length = read_binary("111.png", buffer);

	mysql_write(&mysql, buffer, length);
	printf("length : %d\n", length);

	memset(buffer, 0, BINARY_LENGTH);
	
	length = mysql_read(&mysql, buffer, BINARY_LENGTH);
	write_binary("a.jpg", buffer, length);


	mysql_close(&mysql);

	return 0;

}



