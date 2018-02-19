
clear variables

%%%%% Parameters of the simulation %%%%%
plot_simulation = true; % Bolean to plot or not the simulation
N_fish = 5; % Number of fish
N_robot = 1; % Number of robot
deg_vision = 120; % determine the vision in the ring of the agents in degree (120 => they can see 1/3 of the ring in front of them)
co = .9; % Probability to follow the model or to switch heading ("obedience to the heading model")
cv = 0.901; % Probability to stay in the current cell
Max_neighbors = 3; % Maximal number of influencing neighbors
time_simulation = 1800; % Duration of the simulation
C_T = 40; % Number of cells in the ring
Heading_robot = -1; % direction of the robot (-1 is clockwise)
P_move_robot = 1; % probability of the robot to move in next cell


%%%%% Derived parameters %%%%%

N_agents = N_fish + N_robot; % Number of agents
cell_degree = 360/C_T; % degree by cell
C_P = ceil(deg_vision/cell_degree); % number of cells to look for neighbors in front of the agent current cell

%%%%% Variable agent %%%%%

Heading = sign(randn(1, N_agents)); % Current heading of the agents (-1 or 1)
New_Heading = zeros(1, N_agents); % Store the next heading of the agents
Position = floor(rand(1,N_agents)*C_T); % Position of the agents

%%%%% Save matrices %%%%%

save_Position = zeros(time_simulation, N_agents); % Record the position of the agents
save_Heading = zeros(time_simulation, N_agents); % Record the heading of the agents
save_Polarity = zeros(time_simulation, 1); % Record the polarity of the shoal


%%%%% Simulation %%%%%

for t = 1 : 1 : time_simulation % time loop

    for i = 1 : 1 : N_agents % loop for all agents

        Neighborhood = mod(Position(i) : Heading(i) : Position(i) + Heading(i)*C_P , C_T); % Select the cells to look for potential neighbors of agent i

        Sum_Heading = 0; % sum of heading of neihbors (for equation 2.1)
        n_neighbors = 0; % number of neighbors

        for j = 1 : 1 : C_P+1 % check in successive cells for neighbors

            neighbors = find(Position == Neighborhood(j)); % check for neighbors in cell j
            neighbors(neighbors==i) = []; % delete itself from the neighbors

            if isempty(neighbors) == 0 % if there is a least one neighbor

                neighbors = neighbors(randperm(length(neighbors))); % shuffle the neighbors

                for k = 1 : 1 : length(neighbors) % select them successively
                    n_neighbors = n_neighbors + 1; % increment the number of influential neighbor

                    Sum_Heading = Sum_Heading + Heading( neighbors(k) ); % increment the sum of heading of neihbors (for equation 2.1)

                    if n_neighbors == Max_neighbors % if the maximum number of neighbor is reached -> exit the loop
                        break
                    end
                end
            end

            if n_neighbors == Max_neighbors % if the maximum number of neighbor is reached -> exit the loop
                break
            end
        end

        % Compute equation 2.1
        New_Heading(i) = Heading(i) + Sum_Heading; % add the heading of the focal agent

        if New_Heading(i) == 0
            New_Heading(i) = Heading(i); % if the sum is 0, the previous heading dominates
        else
            New_Heading(i) = sign(New_Heading(i)); % I use the sign function rather than dividing by the norm, it gives the same answer
        end

    end

    save_Position(t,:) = Position; % record the position before update
    save_Heading(t,:) = Heading; % record the heading before update

    New_Heading(1:N_robot) = Heading_robot; % set the heading of the robots

    save_Polarity(t, 1) = abs(sum(Heading))/N_agents; % compute and save the polarity

    rand_Heading = double(rand(1, N_agents)<co); % Probability to follow the model or to switch
    rand_Heading(rand_Heading==0) = -1; % For those that switch, set to -1 to reverse their heading
    rand_Heading(1:N_robot) = 1; % keep the heading of the robot
    Heading = New_Heading .* rand_Heading; % Update the heading

    rand_move = rand(1,N_agents)<(1-cv); % Probability to move in this time step
    rand_move(1:N_robot) = P_move_robot; % the robots always move

    Position = mod(Position + Heading .* rand_move , C_T); % Update the position
end


%%%%% Plot simulation %%%%%

if plot_simulation == 1

    th = linspace(0,2*pi,100);
    r1 = 50+zeros(size(th));
    r2 = 50+N_agents+1+zeros(size(th));

    th2 = linspace(0 + (2*pi)/(2*C_T),2*pi - (2*pi)/(2*C_T),C_T);
    r3 = 50+zeros(size(th2));
    r4 = 50+N_agents+1+zeros(size(th2));

    if N_robot > 0
        for t = 1 : 1 : time_simulation
            polarplot(th,r1, 'b')
            hold on
            polarplot(th,r2, 'b')
            polarplot([th2;th2],[r3;r4],'--b')
            polar_robot = polarplot(deg2rad(save_Position(t,1:N_robot)*cell_degree),50+(1:N_robot),'.r', 'MarkerSize', 14);
            polar_fish = polarplot(deg2rad(save_Position(t,1+N_robot:N_agents)*cell_degree),50+(1+N_robot:N_agents),'.b', 'MarkerSize', 14);
            hold off
            grid off
            pax = gca;
            thetaticks(linspace(0,360 - 360/(C_T),C_T))
            thetaticklabels(0:C_T-1)
            pax.RTickLabels = [];
            legend([polar_robot,polar_fish],'Robot', 'Fish')
            drawnow
            pause(.1)
        end
    else
        for t = 1 : 1 : time_simulation
            polarplot(th,r1, 'b')
            hold on
            polarplot(th,r2, 'b')
            polarplot([th2;th2],[r3;r4],'--b')
            polarplot(deg2rad(save_Position(t,:)*cell_degree),50+(1:N_agents),'.b', 'MarkerSize', 10)
            hold off
            grid off
            pax = gca;
            thetaticks(linspace(0,360 - 360/(C_T),C_T))
            thetaticklabels(0:C_T-1)
            pax.RTickLabels = [];
            drawnow
            pause(.1)
        end
    end
end

%%%%% Proportion clockwise %%%%%

figure
plot(sum(save_Heading==-1,2)/N_agents,'.-')
ylim([0 1])
xlabel('Time')
ylabel('Proportion clockwise')
