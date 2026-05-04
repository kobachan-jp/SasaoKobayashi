import pandas as pd

df = pd.read_csv("Healthcare.csv")

gender_ohe = pd.get_dummies(df['Gender'],prefix='Gender').astype('uint8')

symptom_ohe = df["Symptoms"].str.split(",").apply(lambda x :[s.strip() for s in x]).str.join(",").str.get_dummies(",").add_prefix("Symptoms_").astype("uint8")

disease_ohe = pd.get_dummies(df['Disease'],prefix='Disease').astype('uint8')

final_df  = pd.concat([df[['Patient_ID','Age']],gender_ohe, symptom_ohe,df[['Symptom_Count']],disease_ohe], axis=1)

final_df.to_csv("Healthcare_onehot64.csv", index=False)
print(final_df.shape[1])

