#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

typedef struct { // структура - хранит инфу о типе данных и опперациях 
    void (*add)(void* a, void* b, void* result);
    void (*mult)(void* a, void* b, void* result); // указатели на функции
    void (*scalar)(void* a, float scalar, void* result);
    size_t type_size;  // размер типа данных 
} TypeInfo;




// инты
void intAdd(void* a, void* b, void* result) {
    *((int*)result) = *((int*)a) + *((int*)b); 
}
void intMult(void* a, void* b, void* result) {
    *((int*)result) = *((int*)a) * *((int*)b);
}
void intScalar(void* a, float scalar, void* result) {
    *((int*)result) = *((int*)a) * (int)scalar;
}
TypeInfo TypeInt = {intAdd, intMult, intScalar, sizeof(int)};

// флоты
void floatAdd(void* a, void* b, void* result) {
    *((float*)result) = *((float*)a) + *((float*)b);
}
void floatMultiply(void* a, void* b, void* result) {
    *((float*)result) = *((float*)a) * *((float*)b);
}
void floatScalar(void* a, float scalar, void* result) {
    *((float*)result) = *((float*)a) * scalar;
}
TypeInfo TypeInfoFloat = {floatAdd, floatMultiply, floatScalar, sizeof(float)};

// Структура матрицы
typedef struct {
    int size;       
    char type[10];  
    void **data;    
    TypeInfo* typeInfo; 
} Matrix;

typedef struct {
    Matrix L;
    Matrix U;
} LU;

Matrix create_matrix(int size, const char *type) { 
    Matrix mat;
    mat.size = size;
    strcpy(mat.type, type);

    if (strcmp(type, "int") == 0) {
        mat.typeInfo = &TypeInt;                            
    } else if (strcmp(type, "float") == 0) {
        mat.typeInfo = &TypeInfoFloat;
    } else {
        fprintf(stderr, "не верно указан тип: %s\n", type);
        exit(EXIT_FAILURE); 
    }
    mat.data = (void**)malloc(size * sizeof(void*)); // массив строк указателей
    for (int i = 0; i < size; i++) {
        mat.data[i] = malloc(size * mat.typeInfo->type_size);
    }

    return mat;
}
void free_matrix(Matrix mat) {
    for (int i = 0; i < mat.size; i++) {
        free(mat.data[i]);
    }
    free(mat.data);
}
Matrix create_error_matrix(const char *error_message) {
    Matrix mat;
    mat.size = -1;  
    strcpy(mat.type, "error");
    mat.data = NULL;
    fprintf(stderr, "%s\n", error_message);  // Вывод ошибки
    return mat;
}


// Matrix read_matrix(FILE *file) {
//     int size;
//     char type[10];
//     fscanf(file, "%d", &size);
//     fscanf(file, "%s", type);
//     if (strcmp(type, "int") != 0 && strcmp(type, "float") != 0) {
//         fprintf(stderr, "ошибка: %s\n", type);
//         return create_error_matrix("ошибка");
//     }
//     Matrix mat = create_matrix(size, type);
//     if (strcmp(type, "int") == 0) {
//         int **data = (int **)mat.data;
//         for (int i = 0; i < size; i++) {
//             for (int j = 0; j < size; j++) {
//                 fscanf(file, "%d", &data[i][j]);
//             }
//         }
//     } else if (strcmp(type, "float") == 0) {
//         float **data = (float **)mat.data;
//         for (int i = 0; i < size; i++) {
//             for (int j = 0; j < size; j++) {
//                 fscanf(file, "%f", &data[i][j]);
//             }
//         }
//     }
//     return mat;
// }

void write_matrix(FILE *file, Matrix mat) {
    if (mat.size == -1) {  // Проверка на ошибку
        fprintf(file, "Ошибка: %s\n", mat.type);
        return;
    }
    fprintf(file, "%d\n", mat.size);
    fprintf(file, "%s\n", mat.type);
    if (strcmp(mat.type, "int") == 0) {
        for (int i = 0; i < mat.size; i++) {
            for (int j = 0; j < mat.size; j++) {
                fprintf(file, "%d ", ((int **)mat.data)[i][j]);
            }
            fprintf(file, "\n");
        }
    } else if (strcmp(mat.type, "float") == 0) {
        for (int i = 0; i < mat.size; i++) {
            for (int j = 0; j < mat.size; j++) {
                fprintf(file, "%.2f ", ((float **)mat.data)[i][j]);
            }
            fprintf(file, "\n");
        }
    }
}

Matrix transpose_matrix(Matrix mat) {
    Matrix result = create_matrix(mat.size, mat.type);

    if (strcmp(mat.type, "int") == 0) {
        for (int i = 0; i < mat.size; i++) {
            for (int j = 0; j < mat.size; j++) {
                ((int **)result.data)[j][i] = ((int **)mat.data)[i][j];
            }
        }
    } else if (strcmp(mat.type, "float") == 0) {
        for (int i = 0; i < mat.size; i++) {
            for (int j = 0; j < mat.size; j++) {
                ((float **)result.data)[j][i] = ((float **)mat.data)[i][j];
            }
        }
    }

    return result;
}

Matrix scalarMultyply(Matrix mat, float k){
    Matrix result = create_matrix(mat.size, mat.type);
    for(int i = 0; i<mat.size; i++){
        for (int j = 0; j< mat.size; j++){
            ((float **)result.data)[i][j] = k*((float **)mat.data)[i][j];
        }
    }
    return result;
}
// для lu
Matrix intToFloat (Matrix intMat){
    Matrix floatMat = create_matrix(intMat.size, "float");
    for (int i = 0; i < intMat.size; i++){
        for (int j = 0; j < intMat.size; j++){
            ((float **)floatMat.data)[i][j] = (float)(((int**)intMat.data))[i][j];

        }
    }
    return floatMat;
}

LU decomposition(Matrix mat){
    if (strcmp(mat.type, "int") == 0){
        mat = intToFloat(mat);

    }
    LU lu;
    lu.L = create_matrix(mat.size, mat.type);
    lu.U = create_matrix(mat.size, mat.type);
    

        float **m = (float** )mat.data;
        float **L = (float**) lu.L.data;
        float **U = (float**) lu.U.data;

    
        for (int i =0; i< mat.size; i++){
            for (int j=0; j< mat.size; j++){
                L[i][j]= 0;
                U[i][j]= 0;
            }
            L[i][i] = 1;
        }
        for (int i = 0; i < mat.size; i++){
            for (int j= 0; j < mat.size; j++){
                if (i==0){
                    U[i][j] = m[i][j];
                }
                else if (i != 0.0 && j>=i){
                    float sum = 0;
                    for (int k= 0; k < i; k++){
                        sum += (L[i][k]* U[k][j]);
                        
                    }
                    U[i][j] = m[i][j] - sum;
                }
                else {
                    float sum = 0;
                    for (int k= 0; k < j; k++){
                        sum += (L[i][k]* U[k][j]);
                        
                    }
                    L[i][j]= (m[i][j] - sum)*1/U[j][j];
                }

            }
        } 
    
    return lu;
}

Matrix multiply_matrices(Matrix mat1, Matrix mat2) {
    if (mat1.size != mat2.size || strcmp(mat1.type, mat2.type) != 0) {
        return create_error_matrix("ошибка");
    }

    Matrix result = create_matrix(mat1.size, mat1.type);

    void **data1 = mat1.data;
    void **data2 = mat2.data;
    void **res = result.data; // указ на данные

    for (int i = 0; i < mat1.size; i++) {
        for (int j = 0; j < mat1.size; j++) {
            void *zero = malloc(mat1.typeInfo->type_size);
            memset(zero, 0, mat1.typeInfo->type_size); // заполн нулями
            memcpy((char*)res[i] + j * mat1.typeInfo->type_size, zero, mat1.typeInfo->type_size);
            free(zero);
            for (int k = 0; k < mat1.size; k++) {

                void *temp1 = malloc(mat1.typeInfo->type_size);
                mat1.typeInfo->mult(
                    (char*)data1[i] + k * mat1.typeInfo->type_size,  

                    (char*)data2[k] + j * mat1.typeInfo->type_size, 
                    temp1 
                ); // умножение

                
                void *temp2 = malloc(mat1.typeInfo->type_size);
                mat1.typeInfo->add(
                    (char*)res[i] + j * mat1.typeInfo->type_size,  
                    temp1,  
                    temp2  
                );

                // copy
                memcpy((char*)res[i] + j * mat1.typeInfo->type_size, temp2, mat1.typeInfo->type_size);
                free(temp1);
                free(temp2);
            }
        }
    }
    return result;
}


// через lu
Matrix inverseMatrix(Matrix mat){
    LU lu = decomposition(mat);
    Matrix U = lu.U;
    Matrix L = lu.L;
    float **l = (float**)L.data;
    float **u = (float**)U.data;
    Matrix U_I = create_matrix(mat.size, "float");
    Matrix L_I = create_matrix(mat.size, "float");
    float **l_i = (float**)L_I.data;
    float **u_i = (float**)U_I.data;

    
    for (int i =0; i < mat.size; i++){
        for (int j=0; j< mat.size; j++){
            l_i[i][j]= 0;
            u_i[i][j]= 0;
        }
        l_i[i][i] = 1.0;
    }

    for (int i= mat.size - 1; i>= 0; i--){
        for (int j = i; j < mat.size; j++){
            if (i == j){
                u_i[i][j] = 1/u[i][i];
                
            } else {
                float sum = 0;
                for(int k = i+1; k <=j; k++) {
                    sum += u[i][k] * u_i[k][j];
                }
                u_i[i][j] = -sum/u[i][i];   
            }
        }
    }

    for (int i= 1; i < mat.size; i++){
        for (int j = 0; j < i; j++){
            float sum = 0;
                for(int k = j; k <i ; k++) {
                    sum += l[i][k] * l_i[k][j];
                }
                l_i[i][j] = -sum;
        }   
    }             
    Matrix res = multiply_matrices(U_I, L_I);
    free_matrix(U);
    free_matrix(L);
    free_matrix(U_I);
    free_matrix(L_I);

    return res;
}
     

// void writeMatrixLU( FILE *file, LU lu){
//     write_matrix(file, lu.L);
//     write_matrix(file, lu.U);
// }


float deter(Matrix mat){
    LU res = decomposition(mat);
    Matrix matU = res.U;
    float **m = (float**)matU.data; 
    float det = m[0][0];
    for (int i = 1; i< matU.size; i++){
        det *= m[i][i];
    }
    return det;

}

Matrix add_matrices(Matrix mat1, Matrix mat2) {
    if (mat1.size != mat2.size || strcmp(mat1.type, mat2.type) != 0) {
        return create_error_matrix("ошибка");
    }



    Matrix result = create_matrix(mat1.size, mat1.type);

    for (int i = 0; i < mat1.size; i++) {
        for (int j = 0; j < mat1.size; j++) {
            mat1.typeInfo->add(mat1.data[i] + j, mat2.data[i] + j, result.data[i] + j);
        }
    }

    return result;
}

// через lu
float* SLAU(Matrix A, float* B, int Bsize){
    LU lu = decomposition(A);
    Matrix L = lu.L;
    Matrix U = lu.U;
    float *y = malloc(A.size * sizeof(float));
    float *x = malloc(A.size * sizeof(float));


    
    float **a = (float**)A.data;
    float **l = (float**)L.data;
    float **u = (float**)U.data;
    for (int i = 0; i < A.size; i++){
            float sum = 0.0;
            for (int k = 0; k < i; k++){
                sum += y[k]* l[i][k];
            }
            y[i]= B[i] - sum;
        
    }
    for (int i = A.size - 1; i >= 0; i--){
            float sum = 0.0;
            for (int k = i+1; k < A.size; k++){
                sum += u[i][k]*x[k];
            }
            x[i] = (y[i] - sum)/u[i][i];
    }
    free(y);
    free_matrix(L);
    free_matrix(U);
    return x;
}


Matrix linear_combination(Matrix mat, int string, float *cf) {
    int row_index = string - 1;  

    if (row_index < 0 || row_index >= mat.size) {
        return create_error_matrix("ошибка");
    }
    Matrix result = create_matrix(mat.size, mat.type);

    if (strcmp(mat.type, "int") == 0) {
        for (int i = 0; i < mat.size; i++) {

            for (int j = 0; j < mat.size; j++) {
                ((int **)result.data)[i][j] = ((int **)mat.data)[i][j];
            }
        }

    } else if (strcmp(mat.type, "float") == 0) {
        for (int i = 0; i < mat.size; i++) {
            for (int j = 0; j < mat.size; j++) {

                ((float **)result.data)[i][j] = ((float **)mat.data)[i][j];
            }
        }
    }
    int coeff_index = 0; 
    for (int i = 0; i < mat.size; i++) {
        if (i != row_index) {  // Пропускаем 
            for (int j = 0; j < mat.size; j++) {
                if (strcmp(mat.type, "int") == 0) {
                    ((int **)result.data)[row_index][j] += (int)(cf[coeff_index] * ((int **)mat.data)[i][j]);

                } else if (strcmp(mat.type, "float") == 0) {
                    ((float **)result.data)[row_index][j] += cf[coeff_index] * ((float **)mat.data)[i][j];
                }
            }
            coeff_index++;  // К следующему коэффициенту
        }
    }

    return result;
}

// для слау
float* readFloatArray(int *size){
    char buf[1024];
    float *arr = NULL;
    int count = 0;
    if (fgets(buf, sizeof(buf), stdin) != NULL){
        char *token = strtok(buf,  "\t\n");
        while (token != 0){
            float *temp = realloc(arr, (count + 1) * sizeof(float));
            arr = temp;
            arr[count] = atof(token);
            count++;
            token = strtok(NULL, " \t\n");
        }
    }
    *size = count;
    return arr;

}
void writeMatrixLU(FILE *file, LU lu){
    fprintf(file, "Matrix L:\n");
    write_matrix(file, lu.L);
    fprintf(file, "\nMatrix U:\n");
    write_matrix(file, lu.U);
}


// int main() {
//     FILE *input_file1 = fopen("input1.txt", "r");
//     FILE *input_file2 = fopen("input2.txt", "r");
//     FILE *output_file = fopen("output.txt", "w");
//     if (!input_file1 || !input_file2 || !output_file) {
//         perror("ошибка");
//         return EXIT_FAILURE;
//     }
//     Matrix mat1 = read_matrix(input_file1);
//     Matrix mat2 = read_matrix(input_file2);
//     LU luMatr = decomposition(mat1);
//     float determ = deter(mat1);
    // Matrix SkalarM = scalarMultyply(mat2, 4.4);
    // Matrix transposed1 = transpose_matrix(mat1);
    // Matrix transposed2 = transpose_matrix(mat2);
    // Matrix sum = add_matrices(mat1, mat2);
    // Matrix product = multiply_matrices(mat1, mat2);
    // int target_row;
    // float coefficients[mat1.size - 1];
    // fscanf(input_file1, "%d", &target_row);
    // for (int i = 0; i < mat1.size - 1; i++) {
    //     fscanf(input_file1, "%f", &coefficients[i]);
    // }
    // // Matrix lin_comb = linear_combination(mat1, target_row, coefficients);
    // writeMatriwLU(output_file, luMatr);
    // fprintf(output_file, "%f\n", determ);
    // write_matrix(output_file, SkalarM);
    // write_matrix(output_file, transposed1);
    // write_matrix(output_file, transposed2);
    // write_matrix(output_file, sum);
    // write_matrix(output_file, product);
    // write_matrix(output_file, lin_comb);
    // free_matrix(mat1);
    // free_matrix(mat2);
    // free_matrix(transposed1);
    // free_matrix(transposed2);
    // free_matrix(sum);
    // free_matrix(product);
    // free_matrix(lin_comb);
    //free_matrix(ScalarM);
    //free_matrix(luMatr.L);
    // free_matrix(luMatr.U);
//     fclose(input_file1);
//     fclose(input_file2);
//     fclose(output_file);
//     return 0;
// }


Matrix input_matrix() {
    char type[10];
    int size;
    printf("Enter Matrix type (int or float): ");
    scanf("%s", type);
    printf("Enter Matrix size: ");
    scanf("%d", &size);
    Matrix mat = create_matrix(size, type);
    printf("Enter matrix elements:\n");
    if (strcmp(type, "int") == 0) {
        int **data = (int **)mat.data;
        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++){
                scanf("%d", &data[i][j]);
            }
        }
    } else if (strcmp(type, "float") == 0) {
        float **data = (float **)mat.data;
        for (int i = 0; i < size; i++){
            for (int j = 0; j < size; j++){
                scanf("%f", &data[i][j]);
            }
        }
    }
    return mat;
}
int main() {
    int op;
    printf("Select operation:\n");
    printf("1. LU \n");
    printf("2. Matrix addition\n");
    printf("3. Matrix Multuplucation\n");
    printf("4. Matrix transpose\n");
    printf("5. Matrix scalar multiplication\n");
    printf("6. Determinant\n");
    printf("7. Matrix inverse\n");
    printf("8. SLAU\n");
    printf("Enter number: ");
    scanf("%d", &op);

    Matrix mat1, mat2, result;
    LU luRes;
    float det;
    float scalar;
    switch (op) {
        case 1:
            // LU разложение
            printf("Enter matrix for LU:\n");
            mat1 = input_matrix();
            luRes = decomposition(mat1);
            printf("\nAnswer LU:\n");
            writeMatrixLU(stdout, luRes);
            free_matrix(mat1);
            free_matrix(luRes.L);
            free_matrix(luRes.U);
            break;
        case 2:
            // Сложение матриц
            printf("Enter the first matrix:\n");
            mat1 = input_matrix();
            printf("Enter thr second matrix:\n");
            mat2 = input_matrix();
            result = add_matrices(mat1, mat2);
            printf("\nAnswer addition:\n");
            write_matrix(stdout, result);
            free_matrix(mat1);
            free_matrix(mat2);
            free_matrix(result);
            break;
        case 3:
            // Умножение матриц
            printf("Enter the first matrix:\n");
            mat1 = input_matrix();
            printf("Enter the second matrix:\n");
            mat2 = input_matrix();
            result = multiply_matrices(mat1, mat2);
            printf("\nAnswer multiplication:\n");
            write_matrix(stdout, result);
            free_matrix(mat1);
            free_matrix(mat2);
            free_matrix(result);
            break;
        case 4:
            // Транспонирование 
            printf("Enter matrix:\n");
            mat1 = input_matrix();
            result = transpose_matrix(mat1);
            printf("\nAnswer transpose:\n");
            write_matrix(stdout, result);
            free_matrix(mat1);
            free_matrix(result);
            break;
        case 5:
            // Скаляное 
            printf("Enter matrix:\n");
            mat1 = input_matrix();
            printf("Enter scalar ");
            scanf("%f", &scalar);
            result = scalarMultyply(mat1, scalar);
            printf("\nAnswer:\n");
            write_matrix(stdout, result);
            free_matrix(mat1);
            free_matrix(result);
            break;
        case 6:
            //Детерминант
            printf("Enter Matrix:\n");
            mat1 = input_matrix();
            det = deter(mat1);
            printf("Answer:\n");
            printf("%f", det);
            free_matrix(mat1);
            break;
        case 7:
            // Обратная матрица
            printf("Enter Matrix:\n");
            mat1 = input_matrix();
            result = inverseMatrix(mat1);
            printf("Answer:\n");
            write_matrix(stdout, result);
            free_matrix(mat1);
            free_matrix(result);
            break;
        case 8:
            // Решение СЛАУ
            printf("Enter Matrix:\n");
            mat1 = input_matrix();
            printf("Enter vector:\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            int vecSize;
            float* B = readFloatArray(&vecSize);
            float* x = SLAU(mat1, B, mat1.size);
            for (int i = 0; i < mat1.size; i++) {
                printf("%.4f ", x[i]);
            }
            printf("\n");
            free(B);
            free(x);
            free_matrix(mat1);
            break;
        default:
            printf("Error\n");
            break;
    }
    return 0;
}   
