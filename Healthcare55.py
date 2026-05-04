import pandas as pd

df = pd.read_csv("Healthcare.csv")

symptom_list = (
    df["Symptoms"]
    .str.split(",")
    .explode()
    .str.strip()
)

print(symptom_list.value_counts())


gender_ohe = pd.get_dummies(df['Gender'],prefix='Gender').astype('uint8')

df["Symptoms"] = df["Symptoms"].str.replace(" ", "")
symptom_ohe = df["Symptoms"].str.get_dummies(sep=",").astype("uint8")


disease_ohe = pd.get_dummies(df['Disease'],prefix='Disease').astype('uint8')

final_df  = pd.concat([df[['Patient_ID','Age']],gender_ohe, symptom_ohe,df[['Symptom_Count']],disease_ohe], axis=1)

final_df.to_csv("Healthcare_onehot55.csv", index=False)
print(final_df.shape[1])
print(symptom_ohe.shape[1])



