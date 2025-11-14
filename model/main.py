import torch
from train.train import Train
from networks.mlp_network import MlpNetwork
from data_edit.ArmpiDataset import ArmpiDataset
from torch.utils.data import Dataset, DataLoader, random_split

VAL_SPLIT = 0.2
BATCH_SIZE = 32

if __name__ == '__main__':
    read_data_list = ["bringup_red","bringup_red1","bringup_red2","bringup_red3","bringup_red4"]
    full_dataset = ArmpiDataset(read_data_list)

    val_size = int(len(full_dataset) * VAL_SPLIT)
    train_size = len(full_dataset) - val_size
    train_dataset, val_dataset = random_split(full_dataset, [train_size, val_size])

    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True, num_workers=0)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=0)

    model = MlpNetwork(state_input_dim=full_dataset.state_dim, action_output_dim=full_dataset.action_dim)

    trainer = Train(train_loader,val_loader,model,"model/result/model.pt",epochs=100)
    trainer.train()
